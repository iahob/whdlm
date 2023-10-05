#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "AESEncrypt.h"

//////////////////////////////////////////////////////////////////////////

//����ʱ��
#define TIME_LESS						2									//����ʱ��
#define TIME_DELAY_TIME					3									//��ʱʱ��

//��Ϸʱ��
#define TIME_START_GAME					4									//��ʼʱ��
#define TIME_USER_ADD_SCORE				4									//��עʱ��
#define TIME_USER_COMPARE_CARD			4									//����ʱ��
#define	TIME_USER_FINISH_FLASH			4									//����ʱ��

//��������ԭ��
unsigned char chCommonEncryption[AESENCRYPTION_LENGTH] = 
{
	0x32, 0x43, 0xf6, 0xa8, 
	0x88, 0x5a, 0x30, 0x8d, 
	0x31, 0x31, 0x98, 0xa2, 
	0xe0, 0x37, 0x07, 0x34
};

//��Ϸʱ��
#define IDI_START_GAME					(100)								//��ʼ��ʱ��
#define IDI_USER_ADD_SCORE				(101)								//��ע��ʱ��
#define IDI_USER_COMPARE_CARD			(102)								//���ƶ�ʱ��
#define IDI_DELAY_TIME					(103)								//��ʱ��ʱ��
#define IDI_USER_FINISH_FLASH			(104)								//������ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//��Ϸ����
	m_lStockScore = 0;
	m_wBankerUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	m_cbCardType = 0;

	//��ע��Ϣ
	m_bMingZhu = false;
	m_lCellScore = 0;
	m_lMaxCellScore = 0;
	m_lUserMaxScore = 0;
	m_lCurrentTimes = 0;
	m_cbCurTurnCount = 0;
	m_bShowHand = false;
	m_cbMinLookCardTurnCount = 0;
	m_cbMinCompareCardTurnCount = 0;
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
	ZeroMemory(m_nButtonValue,sizeof(m_nButtonValue));
	//������
	m_nRobotBankStorageMul=0;
	m_lRobotBankGetScore=0;
	m_lRobotBankGetScoreBanker=0;
	ZeroMemory(m_lRobotScoreRange, sizeof(m_lRobotScoreRange));

	//��Ϸ״̬
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));

	//�û��˿�
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbAllHandCardData, sizeof(m_cbAllHandCardData));

	//�ӿڱ���
	m_pIAndroidUserItem=NULL;
	srand((unsigned)time(NULL));   
	
	ZeroMemory(m_chUserAESKey, sizeof(m_chUserAESKey));

	return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ�ӿ�
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//���ýӿ�
bool CAndroidUserItemSink::RepositionSink()
{
	//��Ϸ����
	m_lStockScore = 0;
	m_wBankerUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	m_cbCardType = 0;

	//��ע��Ϣ
	m_bMingZhu = false;
	m_lCellScore = 0;
	m_lMaxCellScore = 0;
	m_lUserMaxScore = 0;
	m_lCurrentTimes = 0;
	m_bShowHand = false;
	m_cbCurTurnCount = 0;
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));

	//��Ϸ״̬
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));

	//�û��˿�
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbAllHandCardData, sizeof(m_cbAllHandCardData));

	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//��ʼ��ʱ��
		{
			//����׼��
			m_pIAndroidUserItem->SendUserReady(NULL, 0);

			return true;
		}
	case IDI_USER_ADD_SCORE:	//��ע��ʱ��
		{
			//ɾ����ʱ��
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);

			//����Ч��
			ASSERT(m_pIAndroidUserItem->GetChairID() == m_wCurrentUser);
			if (m_pIAndroidUserItem->GetChairID() != m_wCurrentUser) return false;
			
			//�������
			bool bGiveUp = false;
			BYTE cbProbability = 0;
			LONGLONG lCurrentScore = 0;
			WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

			//�������
			BYTE cbPlayerCount = 0;
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (m_cbPlayStatus[i] == TRUE) cbPlayerCount++;
			}
			WORD wWinner = EstimateWinner();
			//�Ƿ�����
			if (m_cbRealPlayer[wWinner] == TRUE)
			{
				LONGLONG lTableScore = 0;
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					if (m_cbAndroidStatus[i] == TRUE)
					{
						lTableScore += m_lTableScore[i];
					}
				}

				//�Ƿ��䵽����
				if (lTableScore + m_lMaxCellScore > m_lStockScore) bGiveUp = true;
			}

			if (m_bShowHand)
			{
				if (bGiveUp)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
					return true;
				}
				else
				{
					if (m_bMingZhu == false)
					{
						m_bMingZhu = true;
						m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);
						return true;
					}
					else
					{
						cbProbability = rand() % 10;
						bool bAddScore = false;
						if (wWinner == wMeChairID)
						{
							bAddScore = true;
						}
						else
						{
							BYTE cbAddWeight = 0;

							if (m_cbCardType == CT_BAO_ZI)
								cbAddWeight = 10;
							else if (m_cbCardType == CT_SHUN_JIN)
								cbAddWeight = 8;
							else if (m_cbCardType == CT_JIN_HUA)
								cbAddWeight = 6;
							else if (m_cbCardType == CT_SHUN_ZI)
								cbAddWeight = 2;

							if (cbProbability < cbAddWeight)
								bAddScore = true;
						}

						if (bAddScore)
						{
							//������Ϣ
							CMD_C_AddScore AddScore;
							AddScore.wState = FALSE;
							AddScore.lScore = m_lUserMaxScore - m_lTableScore[wMeChairID];

							//���ܲ���
							AESEncrypt aes(m_chUserAESKey);
							unsigned char chTempEncryption[AESENCRYPTION_LENGTH];
							ZeroMemory(chTempEncryption, sizeof(chTempEncryption));
							CopyMemory(chTempEncryption, chCommonEncryption, sizeof(chTempEncryption));

							aes.Cipher(chTempEncryption);
							CopyMemory(AddScore.chciphertext, chTempEncryption, sizeof(chTempEncryption));

							m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
						}
						else
							m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

					}
				}

				return true;
			}
			//�״β���
			if (m_lTableScore[wMeChairID] == m_lCellScore)
			{
				//�������
				cbProbability = rand()%(10);

				//���Ƹ���
				if (m_bMingZhu == false)
				{
					if (cbProbability < LOOK_CARD)
					{
						m_bMingZhu = true;
						m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);

						return true;
					}
				}
				else
				{
					//ɢ�Ʒ���
					if (m_cbCardType == 0 && wMeChairID != EstimateWinner())
					{
						m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

						return true;
					}
					else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner())
					{
						//��������
						if (cbProbability < GIVE_UP)
						{
							m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

							return true;
						}
					}
				}

				//���ּ�ע
				lCurrentScore = m_lCellScore * GetAddScoreTimes();

				//������Ϣ
				CMD_C_AddScore AddScore;
				AddScore.wState = FALSE;
				AddScore.lScore = lCurrentScore;

				//���ܲ���
				AESEncrypt aes(m_chUserAESKey);
				unsigned char chTempEncryption[AESENCRYPTION_LENGTH];
				ZeroMemory(chTempEncryption, sizeof(chTempEncryption));
				CopyMemory(chTempEncryption, chCommonEncryption, sizeof(chTempEncryption));

				aes.Cipher(chTempEncryption);
				CopyMemory(AddScore.chciphertext, chTempEncryption, sizeof(chTempEncryption));

				m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

				return true;
			}
			else
			{
				//�Ƿ���
				if (m_bMingZhu == false)
				{
					//���ܷ���
					if (bGiveUp == true)
					{
						m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
						return true;
					}

					//�������
					cbProbability = rand()%(10);

					//���Ƹ���
					if (cbProbability < 2 && m_cbCurTurnCount>=m_cbMinLookCardTurnCount)
					{
						m_bMingZhu = true;
						m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);
						return true;
					}

					//�û���ע
					if (m_lTableScore[wMeChairID] < 2 * m_lMaxCellScore)
					{
						//��ע����
						lCurrentScore = m_lCellScore * GetAddScoreTimes();
						
						//������Ϣ
						CMD_C_AddScore AddScore;
						AddScore.wState = FALSE;
						AddScore.lScore = lCurrentScore;

						//���ܲ���
						AESEncrypt aes(m_chUserAESKey);
						unsigned char chTempEncryption[AESENCRYPTION_LENGTH];
						ZeroMemory(chTempEncryption, sizeof(chTempEncryption));
						CopyMemory(chTempEncryption, chCommonEncryption, sizeof(chTempEncryption));

						aes.Cipher(chTempEncryption);
						CopyMemory(AddScore.chciphertext, chTempEncryption, sizeof(chTempEncryption));

						m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

						return true;
					}
					else//���Ƹ���
					{
						//��ע����
						lCurrentScore = 2*m_lCurrentTimes * m_lCellScore;
						
						bool bShowHand = false;
						//���������ע
						if (lCurrentScore + m_lTableScore[wMeChairID] >= m_lUserMaxScore)
						{
							lCurrentScore = m_lUserMaxScore - m_lTableScore[wMeChairID];//���
							bShowHand = true;
						}
		
						//������Ϣ
						CMD_C_AddScore AddScore;
						AddScore.wState = bShowHand?FALSE:TRUE;
						AddScore.lScore = lCurrentScore;

						//���ܲ���
						AESEncrypt aes(m_chUserAESKey);
						unsigned char chTempEncryption[AESENCRYPTION_LENGTH];
						ZeroMemory(chTempEncryption, sizeof(chTempEncryption));
						CopyMemory(chTempEncryption, chCommonEncryption, sizeof(chTempEncryption));

						aes.Cipher(chTempEncryption);
						CopyMemory(AddScore.chciphertext, chTempEncryption, sizeof(chTempEncryption));

						m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

						if (bShowHand == false)
						{
							//������Ϣ
							CMD_C_CompareCard CompareCard;
							ZeroMemory(&CompareCard, sizeof(CompareCard));

							//�ж�����
							if (cbPlayerCount > 2)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
								//���ö�ʱ��
								UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
								m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
							}
							else
							{
								//�������
								for (WORD i = 0; i < GAME_PLAYER; i++)
								{
									if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
									{
										CompareCard.wCompareUser = i;
										break;
									}
								}
								m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
							}
						}


						return true;
					}
				}
				else//�ѿ���
				{
					//���ܷ���
					if (bGiveUp == true)
					{
						m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
						return true;
					}

					//�������
					cbProbability = rand()%(10);

					//ɢ�Ʒ���
					if (m_cbCardType == 0 && wMeChairID != EstimateWinner())
					{
						m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

						return true;
					}
					else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner())
					{
						//��������
						if (cbProbability < GIVE_UP)
						{
							m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

							return true;
						}
					}

					//��ע����
					if ( m_cbCardType < CT_JIN_HUA && m_lTableScore[wMeChairID] < (1 * 2 * m_lMaxCellScore * m_cbCardType)
						|| m_cbCardType == CT_JIN_HUA && m_lTableScore[wMeChairID] < (3 * 2 * m_lMaxCellScore * m_cbCardType)
						|| m_cbCardType > CT_JIN_HUA && m_lTableScore[wMeChairID] < (5 * 2 * m_lMaxCellScore * m_cbCardType))
					{
						//��ע����
						lCurrentScore = m_lCellScore * GetAddScoreTimes();

						//���������ע
						if (lCurrentScore + m_lTableScore[wMeChairID] >= m_lUserMaxScore)
						{
							lCurrentScore = m_lUserMaxScore - m_lTableScore[wMeChairID];//���
						}

						//������Ϣ
						CMD_C_AddScore AddScore;
						AddScore.wState = FALSE;
						AddScore.lScore = lCurrentScore;

						//���ܲ���
						AESEncrypt aes(m_chUserAESKey);
						unsigned char chTempEncryption[AESENCRYPTION_LENGTH];
						ZeroMemory(chTempEncryption, sizeof(chTempEncryption));
						CopyMemory(chTempEncryption, chCommonEncryption, sizeof(chTempEncryption));

						aes.Cipher(chTempEncryption);
						CopyMemory(AddScore.chciphertext, chTempEncryption, sizeof(chTempEncryption));

						m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
					
					
						return true;
					}
					//�û�����
					else
					{
						//��ע����
						lCurrentScore = 4 * m_lCurrentTimes * m_lCellScore;
						
						bool bShowHand = false;
						//���������ע
						if (lCurrentScore + m_lTableScore[wMeChairID] >= m_lUserMaxScore)
						{
							bShowHand = true;
							lCurrentScore = m_lUserMaxScore - m_lTableScore[wMeChairID];
						}

						//������Ϣ
						CMD_C_AddScore AddScore;
						AddScore.wState = bShowHand?FALSE:TRUE;
						AddScore.lScore = lCurrentScore;

						//���ܲ���
						AESEncrypt aes(m_chUserAESKey);
						unsigned char chTempEncryption[AESENCRYPTION_LENGTH];
						ZeroMemory(chTempEncryption, sizeof(chTempEncryption));
						CopyMemory(chTempEncryption, chCommonEncryption, sizeof(chTempEncryption));

						aes.Cipher(chTempEncryption);
						CopyMemory(AddScore.chciphertext, chTempEncryption, sizeof(chTempEncryption));

						m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

						if (bShowHand == false)
						{
							//������Ϣ
							CMD_C_CompareCard CompareCard;
							ZeroMemory(&CompareCard, sizeof(CompareCard));

							//�ж�����
							if (cbPlayerCount > 2)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
								//���ö�ʱ��
								UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
								m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
							}
							else
							{
								//�������
								for (WORD i = 0; i < GAME_PLAYER; i++)
								{
									if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
									{
										CompareCard.wCompareUser = i;
										break;
									}
								}
								m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
							}
						}
						
						return true;
					}
				}
			}

			return false;
		}
	case IDI_USER_COMPARE_CARD:		//���ƶ�ʱ��
		{
			//�������
			WORD wCompareUser = INVALID_CHAIR;
			WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

			//������Ϣ
			CMD_C_CompareCard CompareCard;

			//�������
			while (true)
			{
				wCompareUser = rand()%GAME_PLAYER;
				if (wCompareUser != wMeChairID && m_cbPlayStatus[wCompareUser] == TRUE)
				{
					CompareCard.wCompareUser = wCompareUser;
					break;
				}
			}

			//������Ϣ
			m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));

			return true;	
		}
	case IDI_DELAY_TIME:		//��ʱ��ʱ��
		{
			//ɾ����ʱ��
			m_pIAndroidUserItem->KillGameTimer(IDI_DELAY_TIME);

			//���ö�ʱ��
			UINT nElapse=TIME_LESS + (rand() % TIME_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

			return true;
		}
	case IDI_USER_FINISH_FLASH:	
		{
			m_pIAndroidUserItem->SendSocketData(SUB_C_FINISH_FLASH, NULL, 0);
			return true;
		}
	}

	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//��Ϸ��ʼ
		{
			//��Ϣ����
			return OnSubGameStart(pData, wDataSize);
		}
	case SUB_S_ADD_SCORE:	//�û���ע
		{
			//��Ϣ����
			return OnSubAddScore(pData, wDataSize);
		}
	case SUB_S_GIVE_UP:		//�û�����
		{
			//��Ϣ����
			return OnSubGiveUp(pData, wDataSize);
		}
	case SUB_S_SEND_CARD:	//������Ϣ
		{
			//��Ϣ����
			return true;
		}
	case SUB_S_GAME_END:	//��Ϸ����
		{
			//��Ϣ����
			return OnSubGameEnd(pData, wDataSize);
		}
	case SUB_S_COMPARE_CARD://�û�����
		{
			//��Ϣ����
			return OnSubCompareCard(pData, wDataSize);
		}
	case SUB_S_LOOK_CARD:	//�û�����
		{
			//��Ϣ����
			return OnSubLookCard(pData, wDataSize);
		}
	case SUB_S_PLAYER_EXIT:	//�û�ǿ��
		{
			//��Ϣ����
			return OnSubPlayerExit(pData, wDataSize);
		}
	case SUB_S_OPEN_CARD:	//�û�̯��
		{
			//��Ϣ����
			return OnSubOpenCard(pData, wDataSize);
		}
	case SUB_S_WAIT_COMPARE://�ȴ�����
		{
			//��Ϣ����
			if (wDataSize != sizeof(CMD_S_WaitCompare)) return false;
			CMD_S_WaitCompare * pWaitCompare = (CMD_S_WaitCompare *)pData;

			//�û�Ч��
			ASSERT(pWaitCompare->wCompareUser == m_wCurrentUser);

			////���¶�ʱ
			//if (m_pIAndroidUserItem->GetChairID() == m_wCurrentUser)
			//{
			//	//���ö�ʱ��
			//	UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
			//	m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
			//}
			return true;
		}
	case SUB_S_ANDROID_CARD:
		{
			return OnSubAndroidCard(pData, wDataSize);
		}
	case SUB_S_ANDROID_BANKOPERATOR:
		{
			BankOperate(2);
			return true;
		}
	case SUB_S_UPDATEAESKEY:
		{
			return OnSubUpdateAESKey(pData,wDataSize);
		}
	case SUB_S_RC_TREASEURE_DEFICIENCY:
		{
			return true;
		}
	case SUB_S_TURN_COUNT:
		{
			CMD_S_TurnCount*pack=(CMD_S_TurnCount*)pData;
			m_cbCurTurnCount=pack->cbTurnCount;
			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//����״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��Ϣ����
			CMD_S_StatusFree * pStatusFree = (CMD_S_StatusFree *)pData;

			m_cbMinCompareCardTurnCount = pStatusFree->cbMinCompareCardTurnCount;
			m_cbMinLookCardTurnCount = pStatusFree->cbMinLookCardTurnCount;
			CopyMemory(m_nButtonValue,pStatusFree->nButtonValue,sizeof(m_nButtonValue));
			//��ȡ����
			ReadConfigInformation(&(pStatusFree->CustomAndroid));

			if (!bLookonOther)
			{
				//������Կ
				CopyMemory(m_chUserAESKey, pStatusFree->chUserAESKey, sizeof(m_chUserAESKey));
			}

			CopyMemory(m_szServerName, pStatusFree->szServerName, sizeof(m_szServerName));

			//���в���
			BankOperate(2);

			//���ö�ʱ��
			UINT nElapse=TIME_LESS + (rand() % TIME_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

			return true;
		}
	case GAME_STATUS_PLAY:		//��Ϸ״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay = (CMD_S_StatusPlay *)pData;

			m_cbMinLookCardTurnCount = pStatusPlay->cbMinLookCardTurnCount;
			m_cbMinCompareCardTurnCount = pStatusPlay->cbMinCompareCardTurnCount;
			CopyMemory(m_nButtonValue,pStatusPlay->nButtonValue,sizeof(m_nButtonValue));
			m_bShowHand = pStatusPlay->bShowHand;
			m_cbCurTurnCount = pStatusPlay->cbCurTurnCount;
			//��ȡ����
			ReadConfigInformation(&(pStatusPlay->CustomAndroid));

			if (!bLookonOther)
			{
				//������Կ
				CopyMemory(m_chUserAESKey, pStatusPlay->chUserAESKey, sizeof(m_chUserAESKey));
			}
			
			CopyMemory(m_szServerName, pStatusPlay->szServerName, sizeof(m_szServerName));

			//���в���
			BankOperate(2);

			//�û���Ϣ
			m_wBankerUser = pStatusPlay->wBankerUser;
			m_wCurrentUser = pStatusPlay->wCurrentUser;

			//��ע��Ϣ
			m_lCellScore = pStatusPlay->lCellScore;
			m_lMaxCellScore = pStatusPlay->lMaxCellScore;
			m_lCurrentTimes = pStatusPlay->lCurrentTimes;
			m_lUserMaxScore = pStatusPlay->lUserMaxScore;
			CopyMemory(m_lTableScore, pStatusPlay->lTableScore, sizeof(m_lTableScore));
			m_bMingZhu = pStatusPlay->bMingZhu[m_pIAndroidUserItem->GetChairID()];

			//��Ϸ״̬
			CopyMemory(m_cbPlayStatus, pStatusPlay->cbPlayStatus, sizeof(m_cbPlayStatus));

			//�û��˿�
			CopyMemory(m_cbHandCardData, pStatusPlay->cbHandCardData, sizeof(m_cbHandCardData));

			//�ж��û�
			if (m_pIAndroidUserItem->GetChairID() == m_wCurrentUser)
			{
				//�ж�״̬
				if (pStatusPlay->bCompareState == true)
				{
					//�ȴ�����
					m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, NULL, 0);

					//���ö�ʱ��
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
				}
				else
				{
					//���ö�ʱ��
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
				}
			}

			return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//�û�����
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û���λ
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//��������Ϣ
bool CAndroidUserItemSink::OnSubAndroidCard(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_S_AndroidCard));
	if (wDataSize != sizeof(CMD_S_AndroidCard)) return false;

	//��������
	CMD_S_AndroidCard * pAndroidCard = (CMD_S_AndroidCard *)pBuffer;

	//���ñ���
	m_lStockScore = pAndroidCard->lStockScore;
	CopyMemory(m_cbRealPlayer, pAndroidCard->cbRealPlayer, sizeof(m_cbRealPlayer));
	CopyMemory(m_cbAllHandCardData, pAndroidCard->cbAllHandCardData, sizeof(m_cbAllHandCardData));
	CopyMemory(m_cbAndroidStatus, pAndroidCard->cbAndroidStatus, sizeof(m_cbAndroidStatus));

	return true;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��������
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//�û���Ϣ
	m_wBankerUser = pGameStart->wBankerUser;
	m_wCurrentUser = pGameStart->wCurrentUser;

	//��ע��Ϣ
	m_bMingZhu = false;
	m_lCellScore = pGameStart->lCellScore;
	m_lMaxCellScore = pGameStart->lMaxScore;
	m_lUserMaxScore = pGameStart->lUserMaxScore;
	m_lCurrentTimes = pGameStart->lCurrentTimes;
	CopyMemory(m_cbPlayStatus, pGameStart->cbPlayStatus, sizeof(BYTE)*GAME_PLAYER);

	//�û�״̬
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//��ȡ�û�
		if (m_cbPlayStatus[i] != FALSE)
		{
			m_lTableScore[i] = m_lCellScore;
		}
	}

	//��Ҵ���
	ASSERT(m_pIAndroidUserItem != NULL);
	if (m_pIAndroidUserItem->GetChairID()==pGameStart->wCurrentUser)
	{
		//���ö�ʱ��
		UINT nElapse=TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
	}

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_GiveUp));
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;

	//������Ϣ
	CMD_S_GiveUp * pGiveUp = (CMD_S_GiveUp *)pBuffer;

	//���ñ���
	m_cbRealPlayer[pGiveUp->wGiveUpUser] = FALSE;
	m_cbPlayStatus[pGiveUp->wGiveUpUser] = FALSE;

	return true;
}

//�û���ע
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_AddScore));
	if (wDataSize != sizeof(CMD_S_AddScore)) return false;

	//������Ϣ
	CMD_S_AddScore * pAddScore = (CMD_S_AddScore *)pBuffer;

	//���ñ���
	m_wCurrentUser = pAddScore->wCurrentUser;
	m_lCurrentTimes = pAddScore->lCurrentTimes;
	m_bShowHand = pAddScore->bShowHand;
	m_lTableScore[pAddScore->wAddScoreUser] += pAddScore->lAddScoreCount;
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	//�û�����
	if (wMeChairID==m_wCurrentUser && pAddScore->wCompareState==FALSE)
	{
		//���ö�ʱ��
		UINT nElapse=TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
	}

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubLookCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_LookCard));
	if (wDataSize != sizeof(CMD_S_LookCard)) return false;

	//������Ϣ
	CMD_S_LookCard * pLookCard = (CMD_S_LookCard *)pBuffer;

	//�����û�
	if (m_wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		//�û��˿�
		CopyMemory(m_cbHandCardData, pLookCard->cbCardData, sizeof(m_cbHandCardData));

		//��ȡ����
		m_GameLogic.SortCardList(m_cbHandCardData, MAX_COUNT);
		m_cbCardType = m_GameLogic.GetCardType(m_cbHandCardData, MAX_COUNT);

		//��Ϊɢ��
		if (m_cbCardType < CT_DOUBLE)
		{
			for(BYTE i = 0; i < MAX_COUNT; i++)
			{
				m_cbCardType = 0;
				if (m_GameLogic.GetCardLogicValue(m_cbHandCardData[i]) > 11)
				{
					m_cbCardType = 1;
					break;
				}
			}
		}

		//���ö�ʱ��
		UINT nElapse=TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
	}

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubCompareCard(const void * pBuffer, WORD wDataSize)
{
	//����Ч��
	ASSERT(wDataSize == sizeof(CMD_S_CompareCard));
	if (wDataSize != sizeof(CMD_S_CompareCard)) return false;

	//������Ϣ
	CMD_S_CompareCard * pCompareCard = (CMD_S_CompareCard *)pBuffer;

	//���ñ���
	m_wCurrentUser=pCompareCard->wCurrentUser;
	m_cbPlayStatus[pCompareCard->wLostUser] = FALSE;

	//�������
	BYTE cbUserCount = 0;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i] == TRUE) cbUserCount++;
	}

	//�жϽ���
	if (cbUserCount > 1)
	{
		//��ǰ�û�
		if (m_pIAndroidUserItem->GetChairID() == m_wCurrentUser)
		{
			//���ö�ʱ��
			UINT nElapse=TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse+TIME_DELAY_TIME);
		}
	}
	else if(m_pIAndroidUserItem->GetChairID() == pCompareCard->wCompareUser[0]	|| m_pIAndroidUserItem->GetChairID() == pCompareCard->wCompareUser[1])
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_FINISH_FLASH,TIME_USER_FINISH_FLASH);
	}

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//����Ч��
	ASSERT(wDataSize == sizeof(CMD_S_OpenCard));
	if (wDataSize != sizeof(CMD_S_OpenCard)) return false;

	return true;
}

//�û�ǿ��
bool CAndroidUserItemSink::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_PlayerExit));
	if (wDataSize != sizeof(CMD_S_PlayerExit)) return false;

	//������Ϣ
	CMD_S_PlayerExit * pPlayerExit=(CMD_S_PlayerExit *)pBuffer;

	//���ñ���
	m_cbPlayStatus[pPlayerExit->wPlayerID] = FALSE;

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd = (CMD_S_GameEnd *)pBuffer;

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_COMPARE_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_FINISH_FLASH);

	//�������
	m_lMaxCellScore = 0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//��ʼ����
	WORD wServerType=pGameEnd->wServerType;
	if (wServerType==GAME_GENRE_MATCH)
	{
		return true;
	}
	//��ʼʱ��
	if(pGameEnd->bDelayOverGame==true)
	{
		UINT nElapse=TIME_LESS + (rand() % TIME_DELAY_TIME);
		m_pIAndroidUserItem->SetGameTimer(IDI_DELAY_TIME, nElapse);
	}
	else
	{
		UINT nElapse=TIME_LESS + (rand() % TIME_START_GAME);
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);
	}

	BankOperate(2);

	return true;
}

//������Կ
bool CAndroidUserItemSink::OnSubUpdateAESKey(const void * pBuffer, WORD wDataSize)
{
	ASSERT (wDataSize == sizeof(CMD_S_UpdateAESKey));
	if (wDataSize != sizeof(CMD_S_UpdateAESKey))
	{
		return false;
	}

	CMD_S_UpdateAESKey *pUpdateAESKey = (CMD_S_UpdateAESKey*)pBuffer;

	CopyMemory(m_chUserAESKey, pUpdateAESKey->chUserUpdateAESKey, sizeof(m_chUserAESKey));

	return true;
}

//���в���
void CAndroidUserItemSink::BankOperate(BYTE cbType)
{
	IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	if(pUserItem->GetUserStatus()>=US_SIT)
	{
		if(cbType==1)
		{
			CString strInfo;
			strInfo.Format(TEXT("������״̬���ԣ���ִ�д�ȡ��"));
			NcaTextOut(strInfo, m_szServerName);
			return;
		}
	}

	//��������
	LONGLONG lRobotScore = pUserItem->GetUserScore();

	{
		CString strInfo;
		strInfo.Format(TEXT("[%s] ����(%I64d), �������(%I64d), ȡ������(%I64d)"),pUserItem->GetNickName(),lRobotScore, m_lRobotScoreRange[1], m_lRobotScoreRange[0]);

		if (lRobotScore > m_lRobotScoreRange[1])
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����������(%I64d)"),m_lRobotScoreRange[1]);
			strInfo+=strInfo1;

			NcaTextOut(strInfo, m_szServerName);
		}
		else if (lRobotScore < m_lRobotScoreRange[0])
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����ȡ������(%I64d)"),m_lRobotScoreRange[0]);
			strInfo+=strInfo1;

			NcaTextOut(strInfo, m_szServerName);
		}
		else
		{
			CString strInfo1;
			strInfo1.Format(TEXT("���治ȡ--"));
			strInfo+=strInfo1;

			NcaTextOut(strInfo, m_szServerName);
		}


		//�жϴ�ȡ
		if (lRobotScore > m_lRobotScoreRange[1])
		{			
			LONGLONG lSaveScore=0L;

			lSaveScore = LONGLONG(lRobotScore*m_nRobotBankStorageMul/100);
			if (lSaveScore > lRobotScore) lSaveScore = lRobotScore;
			if (lSaveScore > 0) m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
			LONGLONG lRobotNewScore = pUserItem->GetUserScore();

			CString strInfo;
			strInfo.Format(TEXT("[%s] ִ�д����ǰ���(%I64d)��������(%I64d)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			NcaTextOut(strInfo, m_szServerName);
		}
		else if (lRobotScore < m_lRobotScoreRange[0])
		{
			CString strInfo;
			SCORE lScore=rand()%m_lRobotBankGetScoreBanker+m_lRobotBankGetScore;
			if (lScore > 0)
				m_pIAndroidUserItem->PerformTakeScore(lScore);

			LONGLONG lRobotNewScore = pUserItem->GetUserScore();
			strInfo.Format(TEXT("[%s] ִ��ȡ�ȡ��ǰ���(%I64d)��ȡ�����(%I64d)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			NcaTextOut(strInfo, m_szServerName);
		}
	}
}

//��ȡ����
void CAndroidUserItemSink::ReadConfigInformation(tagCustomAndroid *pCustomAndroid)
{
	m_lRobotScoreRange[0] = pCustomAndroid->lRobotScoreMin;
	m_lRobotScoreRange[1] = pCustomAndroid->lRobotScoreMax;

	if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])	
		m_lRobotScoreRange[1] = m_lRobotScoreRange[0];

	m_lRobotBankGetScore = pCustomAndroid->lRobotBankGet;
	m_lRobotBankGetScoreBanker = pCustomAndroid->lRobotBankGetBanker;
	m_nRobotBankStorageMul = pCustomAndroid->lRobotBankStoMul;

	if (m_nRobotBankStorageMul<0||m_nRobotBankStorageMul>100) 
		m_nRobotBankStorageMul =20;
}

//�ƶ�ʤ��
WORD CAndroidUserItemSink::EstimateWinner()
{
	//�����˿�
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData, m_cbAllHandCardData, sizeof(cbUserCardData));

	//�����˿�
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i], MAX_COUNT);
	}

	//��������
	WORD wWinUser = INVALID_CHAIR;

	//��������
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//�û�����
		if (m_cbPlayStatus[i] == FALSE) continue;

		//�����û�
		if (wWinUser == INVALID_CHAIR)
		{
			wWinUser=i;
			continue;
		}

		//�Ա��˿�
		if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinUser], MAX_COUNT,m_bShowHand) >= TRUE)
		{
			wWinUser = i;
		}
	}

	return wWinUser;
}

//��ע����
LONGLONG CAndroidUserItemSink::GetAddScoreTimes()
{
	ASSERT(m_lCurrentTimes <= m_nButtonValue[3] && m_lCurrentTimes >= 0);
	LONGLONG lTimes=0;

	if(m_lCurrentTimes>=m_nButtonValue[3])
		 lTimes=m_lCurrentTimes;
	else if(m_lCurrentTimes>=m_nButtonValue[2])
		lTimes= m_nButtonValue[3];
	else if(m_lCurrentTimes>=m_nButtonValue[1])
		lTimes= m_nButtonValue[(rand()%2)+2];
	else if(m_lCurrentTimes>=m_nButtonValue[0])
		lTimes= m_nButtonValue[(rand()%3)+1];
	else
		lTimes=m_nButtonValue[rand()%4];

	ASSERT(lTimes != 0);

	if (m_bMingZhu)
		lTimes *= 2;

	return lTimes;
}
//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
