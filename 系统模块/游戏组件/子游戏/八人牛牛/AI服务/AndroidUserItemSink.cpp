#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////

//����ʱ��
#define TIME_LESS						1									//����ʱ��

//��Ϸʱ��
#define TIME_USER_CALL_BANKER			3									//��ׯʱ��
#define TIME_USER_START_GAME			3									//��ʼʱ��
#define TIME_USER_ADD_SCORE				3									//��עʱ��
#define TIME_USER_OPEN_CARD				3									//̯��ʱ��

//��Ϸʱ��
#define IDI_START_GAME					(100)									//��ʼ��ʱ��
#define IDI_CALL_BANKER					(101)									//��ׯ��ʱ��
#define IDI_USER_ADD_SCORE				(102)									//��ע��ʱ��
#define IDI_OPEN_CARD					(103)									//���ƶ�ʱ��
#define IDI_CHATMSG						(106)									//���춨ʱ��

#define MAX_CHAT_SHORT					10									//���������Ŀ

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	m_lTurnMaxScore = 0;
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbFourCardData, sizeof(m_cbFourCardData));

	m_nRobotBankStorageMul = 0;
	m_lRobotBankGetScore = 0;
	m_lRobotBankGetScoreBanker = 0;
	ZeroMemory(m_lRobotScoreRange, sizeof(m_lRobotScoreRange));

	m_cbDynamicJoin = FALSE;

	m_bgtConfig = BGT_DESPOT_;
	m_btConfig = BT_FREE_;
	m_stConfig = ST_SENDFOUR_;
	m_gtConfig = GT_HAVEKING_;
	m_ctConfig = CT_CLASSIC_;

	m_lFreeConfig[0] = 200;
	m_lFreeConfig[1] = 500;
	m_lFreeConfig[2] = 800;
	m_lFreeConfig[3] = 1100;
	m_lFreeConfig[4] = 1500;

	ZeroMemory(m_lPercentConfig, sizeof(m_lPercentConfig));
	m_wBgtRobNewTurnChairID = INVALID_CHAIR;
	m_lCellScore = 0;

	//�ӿڱ���
	m_pIAndroidUserItem = NULL;
	srand((unsigned)time(NULL));

	return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink, Guid, dwQueryVer);
	return NULL;
}

//��ʼ�ӿ�
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIAndroidUserItem = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IAndroidUserItem);
	if (m_pIAndroidUserItem == NULL)
	{
		return false;
	}

	return true;
}

//���ýӿ�
bool CAndroidUserItemSink::RepositionSink()
{
	m_lTurnMaxScore = 0;
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbFourCardData, sizeof(m_cbFourCardData));
	m_wBgtRobNewTurnChairID = INVALID_CHAIR;
	m_lCellScore = 0;

	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	try
	{
		switch (nTimerID)
		{

		case IDI_CHATMSG:
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);
			SendAIChatMessage();

			return true;
		}
		case IDI_START_GAME:		//��ʼ��ʱ��
		{
			if (m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus() < US_READY)
			{
			//����׼��
			m_pIAndroidUserItem->SendUserReady(NULL, 0);
			}

			return true;
		}
		case IDI_CALL_BANKER:		//��ׯ��ʱ
		{
			//���ñ���
			CMD_C_CallBanker CallBanker;
			ZeroMemory(&CallBanker, sizeof(CallBanker));

			//
			if (m_stConfig == ST_SENDFOUR_)
			{
				BYTE cbPlayerCount = 0;
				BYTE cbMaxCardTypeSingleArray[GAME_PLAYER];
				memset(cbMaxCardTypeSingleArray, INVALID_BYTE, sizeof(cbMaxCardTypeSingleArray));

				//������ҵ�Ȩ��
				WORD wOtherChange = 0;

				tagSendFourCallBanker SendFourCallBanker;

				for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
				{
					IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(wChairID);
					if (pIServerUserItem && pIServerUserItem->IsClientReady() && pIServerUserItem->GetUserStatus() >= US_PLAYING)
					{
						cbPlayerCount++;

						cbMaxCardTypeSingleArray[wChairID] = GetMaxCardTypeSingle(wChairID);

						if (wChairID != m_pIAndroidUserItem->GetChairID() && cbMaxCardTypeSingleArray[wChairID] != INVALID_BYTE)
						{
							wOtherChange += SendFourCallBanker.wCallBankerChanceArray[(m_ctConfig == CT_CLASSIC_ ? 0 : 1)][cbMaxCardTypeSingleArray[wChairID]];
						}
					}
				}

				//���ĵ���ģʽ�£����������֪��������������������������
				BYTE cbMaxCardTypeSingle = cbMaxCardTypeSingleArray[m_pIAndroidUserItem->GetChairID()];
				BYTE cbRandVal = rand() % 100;
			
				if (cbPlayerCount <= 1)
				{
					cbPlayerCount = 2;
				}

				//�Լ���Ȩ��
				WORD wMySelfChance = SendFourCallBanker.wCallBankerChanceArray[(m_ctConfig == CT_CLASSIC_ ? 0 : 1)][cbMaxCardTypeSingle] * (cbPlayerCount - 1) + cbPlayerCount / 2;

				WORD wCallBankerChance = 0;
				if (wMySelfChance + wOtherChange == 0)
				{
					wCallBankerChance = 30;
				}
				else
				{
					wCallBankerChance = (WORD)((LONGLONG)wMySelfChance / (LONGLONG)(wMySelfChance + wOtherChange) * 100);
				}
				
				//����Լ�ǰ��4���ư�����С����һ����ׯ
				bool bHaveKing = false;
				for (WORD i = 0; i < 4; i++)
				{
					if (m_cbFourCardData[m_pIAndroidUserItem->GetChairID()][i] == 0x4E
					 || m_cbFourCardData[m_pIAndroidUserItem->GetChairID()][i] == 0x4F)
					{
						bHaveKing = true;
						break;
					}
				}

				if (bHaveKing)
				{
					wCallBankerChance = 100;
				}
				
				//����Լ�ǰ��4����Ϊը����һ����ׯ
				bool bHaveBombType = true;
				BYTE cbOriginalCardData = m_cbFourCardData[m_pIAndroidUserItem->GetChairID()][0];
				for (WORD i = 1; i < 4; i++)
				{
					if (cbOriginalCardData != m_cbFourCardData[m_pIAndroidUserItem->GetChairID()][i])
					{
						bHaveBombType = false;
						break;
					}
				}

				if (bHaveBombType)
				{
					wCallBankerChance = 100;
				}

				//����Լ�ǰ��4����Ϊ��ţ����ׯ
				if (wCallBankerChance != 100 && !IsFourCardTypeNiu(m_cbFourCardData[m_pIAndroidUserItem->GetChairID()], 4, m_ctConfig))
				{
					wCallBankerChance = 0;
				}
				
				//��ȡȨ������
				BYTE cbChanceIndex = GetChanceIndex(SendFourCallBanker.cbBankerTimesArray[cbMaxCardTypeSingle], MAX_CALLBANKERTIMES-1);
				BYTE cbBankerTimes = cbChanceIndex + 1;

				CallBanker.bBanker = (m_wBgtRobNewTurnChairID != INVALID_CHAIR ? true : cbRandVal < wCallBankerChance);
				if (CallBanker.bBanker == false)
				{
					CallBanker.cbBankerTimes = 0;
				}
				else
				{
					//������ׯ��1��
					CallBanker.cbBankerTimes = (m_bgtConfig == BGT_FREEBANKER_ ? 1 : cbBankerTimes);
				}
			}
			else
			{
			CallBanker.bBanker = (m_wBgtRobNewTurnChairID != INVALID_CHAIR ? true : rand() % 100 > 50);
			if (CallBanker.bBanker == false)
			{
				CallBanker.cbBankerTimes = 0;
			}
			else
			{
				//������ׯ��1��
				CallBanker.cbBankerTimes = (m_bgtConfig == BGT_FREEBANKER_ ? 1 : rand() % 4 + 1);
			}
			}

			//������Ϣ
			m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_BANKER, &CallBanker, sizeof(CallBanker));

			return true;
		}
		case IDI_USER_ADD_SCORE:	//��ע��ʱ
		{
			CMD_C_AddScore AddScore;
			ZeroMemory(&AddScore, sizeof(AddScore));

			//ASSERT(m_lTurnMaxScore > 0L);
			if (m_lTurnMaxScore <= 0)
				return true;

			LONGLONG lAddScore = 0LL;

			if (m_btConfig == BT_FREE_)
			{
				tagSendFourAddScore SendFourAddScore;

				//��ȡȨ������
				BYTE cbChanceIndex = GetChanceIndex(SendFourAddScore.cbAddScoreArray[GetMaxCardTypeSingle(m_pIAndroidUserItem->GetChairID())], MAX_ADDSCORE);

				//�ĳ�4��������ҷ��������Ǳ���
				lAddScore = m_lFreeConfig[cbChanceIndex] * m_lCellScore;

				if (lAddScore == 0 || lAddScore > m_lTurnMaxScore)
				{
					lAddScore = m_lTurnMaxScore;
				}
			}
			else if (m_btConfig == BT_PENCENT_)
			{
				lAddScore = m_lPercentConfig[rand() % (MAX_CONFIG - 1)] * m_lTurnMaxScore / 100;

				if (lAddScore == 0)
				{
					lAddScore = m_lTurnMaxScore;
				}
			}

			AddScore.lScore = lAddScore;

			//������Ϣ
			m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

			return true;
		}
		case IDI_OPEN_CARD:			//���ƶ�ʱ
		{
			//������Ϣ
			CMD_C_OpenCard OpenCard;
			ZeroMemory(&OpenCard, sizeof(OpenCard));

			//��ȡ�������
			m_GameLogic.GetOxCard(m_cbHandCardData, MAX_CARDCOUNT);
			CopyMemory(OpenCard.cbCombineCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
			m_pIAndroidUserItem->SendSocketData(SUB_C_OPEN_CARD, &OpenCard, sizeof(OpenCard));

			return true;
		}
		}
	}
	catch (...)
	{
		CString cs;
		cs.Format(TEXT("�쳣ID=%d"), nTimerID);
		CTraceService::TraceString(cs, TraceLevel_Debug);
	}

	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//�Թ�AI��������Ϣ
	if (m_pIAndroidUserItem->GetChairID() >= GAME_PLAYER)
		return true;

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
	case SUB_S_PLAYER_EXIT:	//�û�ǿ��
	{
		//��Ϣ����
		return OnSubPlayerExit(pData, wDataSize);
	}
	case SUB_S_SEND_CARD:	//������Ϣ
	{
		//��Ϣ����
		return OnSubSendCard(pData, wDataSize);
	}
	case SUB_S_GAME_END:	//��Ϸ����
	{
		//��Ϣ����
		return OnSubGameEnd(pData, wDataSize);
	}
	case SUB_S_OPEN_CARD:	//�û�̯��
	{
		//��Ϣ����
		return OnSubOpenCard(pData, wDataSize);
	}
	case SUB_S_CALL_BANKER:	//�û���ׯ
	{
		//��Ϣ����
		return OnSubCallBanker(pData, wDataSize);
	}
	case SUB_S_CALL_BANKERINFO:	//�û���ׯ��Ϣ
	{
		//��Ϣ����
		return OnSubCallBankerInfo(pData, wDataSize);
	}
	case SUB_S_ADMIN_STORAGE_INFO:
	case SUB_S_RECORD:
	{
		return true;
	}
	case SUB_S_ANDROID_BANKOPER:
	{
		BankOperate(2);
		return true;
	}
	case SUB_S_ANDROID_USEROVERTIME:
	{
		m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OTHER_OVERTIME_CT);

		UINT nElapse = rand() % 2 + 1;
		m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);

		return true;
	}
	case SUB_S_ANDROID_NOTIFYWIN:
	{
		m_pIAndroidUserItem->SetAndroidChatType(ANDROID_SERIALWIN_CT);

		UINT nElapse = rand() % 2 + 1;
		m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);

		return true;
	}
	case SUB_S_ANDROID_READY:
	{
		if (m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus() < US_READY)
		{
			//��ʼʱ��
			UINT nElapse = TIME_LESS + (rand() % TIME_USER_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);
		}

		return true;
	}
	case SUB_S_SEND_FOUR_CARD:
	{
		//��Ϣ����
		return OnSubSendFourCard(pData, wDataSize);
	}
	default:
		return true;
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//
	if (wSubCmdID == SUB_GF_USER_CHAT)
	{
		//��������
		CMD_GF_S_UserChat *pUserChat = (CMD_GF_S_UserChat *)pData;

		CString strPhrase0 = TEXT("�ټ��ˣ��һ������ҵ�");
		CString strPhrase1 = TEXT("��λ���ǲ�����˼����Ҫ����");

		TCHAR szShortcut[LEN_USER_CHAT] = TEXT("");

		//��ȡ��Ϣ
		WORD wHeadSize = wDataSize - (sizeof(CMD_GF_S_UserChat)-sizeof(pUserChat->szChatString));
		_sntprintf(szShortcut, wHeadSize, TEXT("%s"), pUserChat->szChatString);

		CString strCmp;
		strCmp.Format(TEXT("%s"), szShortcut);

		if (strCmp == strPhrase0 || strCmp == strPhrase1)
		{
			m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OTHER_STANDUP_CT);

			UINT nElapse = rand() % 4 + 4;
			m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
		}

	}

	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_TK_FREE:		//����״̬
	{
		//Ч������
		if (wDataSize != sizeof(CMD_S_StatusFree))
		{
			return false;
		}

		//��Ϣ����
		CMD_S_StatusFree * pStatusFree = (CMD_S_StatusFree *)pData;


		ReadConfigInformation(&(pStatusFree->CustomAndroid));

		BankOperate(2);

		m_bgtConfig = pStatusFree->bgtConfig;
		m_btConfig = pStatusFree->btConfig;
		m_stConfig = pStatusFree->stConfig;
		m_gtConfig = pStatusFree->gtConfig;
		m_ctConfig = pStatusFree->ctConfig;

		m_lCellScore = pStatusFree->lCellScore;

		//��ʼʱ��
		UINT nElapse = TIME_LESS + (rand() % TIME_USER_START_GAME);
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

		return true;
	}
	case GS_TK_CALL:	// ��ׯ״̬
	{
		//Ч������
		if (wDataSize != sizeof(CMD_S_StatusCall))
		{
			return false;
		}

		CMD_S_StatusCall * pStatusCall = (CMD_S_StatusCall *)pData;

		m_cbDynamicJoin = pStatusCall->cbDynamicJoin;

		ReadConfigInformation(&(pStatusCall->CustomAndroid));

		BankOperate(2);

		m_bgtConfig = pStatusCall->bgtConfig;
		m_btConfig = pStatusCall->btConfig;
		m_stConfig = pStatusCall->stConfig;
		m_gtConfig = pStatusCall->gtConfig;
		m_ctConfig = pStatusCall->ctConfig;

		m_lCellScore = pStatusCall->lCellScore;

		ASSERT(m_bgtConfig == BGT_ROB_);

		if (m_cbDynamicJoin == FALSE && pStatusCall->cbCallBankerStatus[m_pIAndroidUserItem->GetChairID()] == FALSE)
		{
			//��ׯʱ��			
			UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_CALL_BANKER);			
			m_pIAndroidUserItem->SetGameTimer(IDI_CALL_BANKER, nElapse);

			if (nElapse >= 6)
			{
				m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

				nElapse = rand() % 4 + 4;
				m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
		}
		}

		return true;
	}
	case GS_TK_SCORE:	//��ע״̬
	{
		//Ч������
		if (wDataSize != sizeof(CMD_S_StatusScore))
		{
			return false;
		}

		CMD_S_StatusScore * pStatusScore = (CMD_S_StatusScore *)pData;

		m_cbDynamicJoin = pStatusScore->cbDynamicJoin;

		ReadConfigInformation(&(pStatusScore->CustomAndroid));

		BankOperate(2);

		m_bgtConfig = pStatusScore->bgtConfig;
		m_btConfig = pStatusScore->btConfig;
		m_stConfig = pStatusScore->stConfig;
		m_gtConfig = pStatusScore->gtConfig;
		m_ctConfig = pStatusScore->ctConfig;

		m_lCellScore = pStatusScore->lCellScore;

		CopyMemory(m_lFreeConfig, pStatusScore->lFreeConfig, sizeof(m_lFreeConfig));
		CopyMemory(m_lPercentConfig, pStatusScore->lPercentConfig, sizeof(m_lPercentConfig));

		//���ñ���
		m_lTurnMaxScore = pStatusScore->lTurnMaxScore;
		WORD wMeChairId = m_pIAndroidUserItem->GetChairID();

		//���ó���
		if (pStatusScore->lTurnMaxScore > 0 && pStatusScore->lTableScore[wMeChairId] == 0 && m_cbDynamicJoin == FALSE
			&& wMeChairId != pStatusScore->wBankerUser)
		{
			//��עʱ��
			UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_ADD_SCORE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);

			if (nElapse >= 5)
			{
				m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

				nElapse = rand() % 4 + 4;
				m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
		}
		}

		return true;
	}
	case GS_TK_WAITEND:
	case GS_TK_PLAYING:	//��Ϸ״̬
	{
		//Ч������
		if (wDataSize != sizeof(CMD_S_StatusPlay))
		{
			return false;
		}

		CMD_S_StatusPlay * pStatusPlay = (CMD_S_StatusPlay *)pData;

		m_cbDynamicJoin = pStatusPlay->cbDynamicJoin;

		ReadConfigInformation(&(pStatusPlay->CustomAndroid));

		BankOperate(2);

		m_bgtConfig = pStatusPlay->bgtConfig;
		m_btConfig = pStatusPlay->btConfig;
		m_stConfig = pStatusPlay->stConfig;
		m_gtConfig = pStatusPlay->gtConfig;
		m_ctConfig = pStatusPlay->ctConfig;

		m_lCellScore = pStatusPlay->lCellScore;

		CopyMemory(m_lFreeConfig, pStatusPlay->lFreeConfig, sizeof(m_lFreeConfig));
		CopyMemory(m_lPercentConfig, pStatusPlay->lPercentConfig, sizeof(m_lPercentConfig));

		//���ñ���
		m_lTurnMaxScore = pStatusPlay->lTurnMaxScore;
		WORD wMeChiarID = m_pIAndroidUserItem->GetChairID();

		CopyMemory(m_cbHandCardData, pStatusPlay->cbHandCardData[wMeChiarID], MAX_CARDCOUNT);

		//�ؼ�����
		if (pStatusPlay->bOpenCard[wMeChiarID] == false && m_cbDynamicJoin == FALSE)
		{
			//����ʱ��
			UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_OPEN_CARD);
			m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse);

			if (nElapse >= 5)
			{
				m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

				nElapse = rand() % 4 + 4;
				m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
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

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize != sizeof(CMD_S_GameStart))
	{
		return false;
	}

	CMD_S_GameStart * pGameStart = (CMD_S_GameStart *)pBuffer;

	//���ñ���
	m_lTurnMaxScore = pGameStart->lTurnMaxScore;

	CopyMemory(m_cbHandCardData, pGameStart->cbCardData[m_pIAndroidUserItem->GetChairID()], sizeof(m_cbHandCardData));

	m_bgtConfig = pGameStart->bgtConfig;
	m_btConfig = pGameStart->btConfig;
	m_stConfig = pGameStart->stConfig;
	m_gtConfig = pGameStart->gtConfig;

	CopyMemory(m_lFreeConfig, pGameStart->lFreeConfig, sizeof(m_lFreeConfig));
	CopyMemory(m_lPercentConfig, pGameStart->lPercentConfig, sizeof(m_lPercentConfig));

	//���ó���
	if (pGameStart->lTurnMaxScore > 0 && m_cbDynamicJoin == FALSE && m_pIAndroidUserItem->GetChairID() != pGameStart->wBankerUser)
	{
		//��עʱ��(�ͻ��˶���1~4s)
		UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);

		if (nElapse >= 5)
		{
			m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

			nElapse = rand() % 4 + 4;
			m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
	}
	}

	//У�������Ϸ״̬
	if (m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_ENTERGAME))
	{
		m_pIAndroidUserItem->SetAndroidChatType(ANDROID_ENTERGAME_CT);

		UINT nElapse = rand() % 2 + 1;
		m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
	}

	return true;
}

//�û���ע
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//У���뿪��Ϸ״̬
	//if (m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
	//{
	//	m_pIAndroidUserItem->SetAndroidChatType(ANDROID_STANDUP_CT);

	//	UINT nElapse = rand() % 4 + 4;
	//	m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
	//}

	//AI�������û���ע
	return true;
}

//�û�ǿ��
bool CAndroidUserItemSink::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
	//AI�������û�ǿ��
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize != sizeof(CMD_S_SendCard))
	{
		return false;
	}

	CMD_S_SendCard * pSendCard = (CMD_S_SendCard *)pBuffer;

	//��������
	WORD wMeChiarID = m_pIAndroidUserItem->GetChairID();
	CopyMemory(m_cbHandCardData, pSendCard->cbCardData[wMeChiarID], sizeof(m_cbHandCardData));

	//����ʱ��
	if (m_cbDynamicJoin == FALSE)
	{
		UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_OPEN_CARD);
		m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse);

		if (nElapse >= 5)
		{
			m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

			nElapse = rand() % 4 + 4;
			m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
	}
	}

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(CMD_S_GameEnd))
	{
		return false;
	}

	CMD_S_GameEnd * pGameEnd = (CMD_S_GameEnd *)pBuffer;

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_CALL_BANKER);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	m_pIAndroidUserItem->KillGameTimer(IDI_OPEN_CARD);

	BankOperate(2);

	UINT nElapse = TIME_LESS + (rand() % TIME_USER_START_GAME) + 11;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

	//�������
	m_lTurnMaxScore = 0;
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));

	m_cbDynamicJoin = FALSE;
	ZeroMemory(m_cbFourCardData, sizeof(m_cbFourCardData));

	return true;
}

//�û�̯��
bool CAndroidUserItemSink::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize != sizeof(CMD_S_Open_Card))
	{
		return false;
	}

	CMD_S_Open_Card * pOpenCard = (CMD_S_Open_Card *)pBuffer;

	return true;
}

//�û���ׯ
bool CAndroidUserItemSink::OnSubCallBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize != sizeof(CMD_S_CallBanker))
	{
		return false;
	}

	CMD_S_CallBanker * pCallBanker = (CMD_S_CallBanker *)pBuffer;
	m_wBgtRobNewTurnChairID = pCallBanker->wBgtRobNewTurnChairID;

	if (m_cbDynamicJoin == FALSE)
	{
		if ((pCallBanker->wBgtRobNewTurnChairID == INVALID_CHAIR) || (pCallBanker->wBgtRobNewTurnChairID != INVALID_CHAIR && pCallBanker->wBgtRobNewTurnChairID == m_pIAndroidUserItem->GetChairID()))
		{
			//��ׯʱ��
			UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_CALL_BANKER);
			m_pIAndroidUserItem->SetGameTimer(IDI_CALL_BANKER, nElapse);

			if (nElapse >= 6)
			{
				m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

				nElapse = rand() % 4 + 4;
				m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
		}
	}
	}

	return true;
}

//�û���ׯ��Ϣ
bool CAndroidUserItemSink::OnSubCallBankerInfo(const void * pBuffer, WORD wDataSize)
{
	//AI�������û���ׯ��Ϣ
	return true;
}
//�ɷ��˿�
bool CAndroidUserItemSink::OnSubSendFourCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize != sizeof(CMD_S_SendFourCard))
				return false;

	CMD_S_SendFourCard * pCallBanker = (CMD_S_SendFourCard *)pBuffer;
	CopyMemory(m_cbFourCardData, pCallBanker->cbCardData, sizeof(m_cbFourCardData));
	return true;
}
//���в���
void CAndroidUserItemSink::BankOperate(BYTE cbType)
{
	//������Ϸ�ڽ����ϲ��ܿ��������仯
	return;
}
			
//д��־�ļ�
void CAndroidUserItemSink::WriteInfo(LPCTSTR pszString)
{
	return;

	//������������
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName = TEXT("������ţţAI��ȡ���¼.txt");
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
}

//��ȡ����
void CAndroidUserItemSink::ReadConfigInformation(tagCustomAndroid *pCustomAndroid)
{
	m_lRobotScoreRange[0] = pCustomAndroid->lRobotScoreMin;
	m_lRobotScoreRange[1] = pCustomAndroid->lRobotScoreMax;

	if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])
	{
		m_lRobotScoreRange[1] = m_lRobotScoreRange[0];
	}
		
	m_lRobotBankGetScore = pCustomAndroid->lRobotBankGet;
	m_lRobotBankGetScoreBanker = pCustomAndroid->lRobotBankGetBanker;
	m_nRobotBankStorageMul = pCustomAndroid->lRobotBankStoMul;

	if (m_nRobotBankStorageMul<0 || m_nRobotBankStorageMul>100)
	{
		m_nRobotBankStorageMul = 20;
	}
}

//��������
void CAndroidUserItemSink::SendAIChatMessage()
{
	m_pIAndroidUserItem->SendChatMessage(0);
}

//���ĵ���ģʽ�£����������֪��������������������������
BYTE CAndroidUserItemSink::GetMaxCardTypeSingle(WORD wChairID)
{
	//�˿�����
	CList<BYTE, BYTE &> cardlist;
	cardlist.RemoveAll();

	//����С��
	if (m_gtConfig == GT_HAVEKING_)
	{
		for (WORD i = 0; i < 54; i++)
		{
			cardlist.AddTail(m_GameLogic.m_cbCardListDataHaveKing[i]);
		}
	}
	else if (m_gtConfig == GT_NOKING_)
	{
		for (WORD i = 0; i < 52; i++)
		{
			cardlist.AddTail(m_GameLogic.m_cbCardListDataNoKing[i]);
		}
	}

	//ɾ���˿� ��ɾ��ǰ��4�ţ��������һ�ţ�
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		for (WORD j = 0; j < MAX_CARDCOUNT - 1; j++)
		{
			if (m_cbFourCardData[i][j] != 0)
			{
				POSITION ptListHead = cardlist.GetHeadPosition();
				POSITION ptTemp;
				BYTE cbCardData = INVALID_BYTE;

				//��������
				while (ptListHead)
				{
					ptTemp = ptListHead;
					if (cardlist.GetNext(ptListHead) == m_cbFourCardData[i][j])
					{
						cardlist.RemoveAt(ptTemp);
						break;
					}
				}
			}
		}
	}
	
	return m_GameLogic.GetMaxCardTypeSingle(cardlist, m_cbFourCardData[wChairID], m_ctConfig);	
}

//��ȡȨ������
BYTE CAndroidUserItemSink::GetChanceIndex(BYTE cbChanceArray[], BYTE cbChanceCount)
{
	int nPJRateTotal = 0;

	for (WORD i = 0; i < cbChanceCount; i++)
	{
		nPJRateTotal += cbChanceArray[i];
	}

	int nRandNum = 0;
	BYTE cbCurChip = 0;
	WORD wMyID = m_pIAndroidUserItem->GetChairID();

	static int nPJStFluc = 1;			
	if (nPJRateTotal == 0)
	{
		nPJRateTotal = 1;
	}
	nRandNum = (rand() + wMyID + nPJStFluc * 3) % nPJRateTotal;
	for (int i = 0; i < cbChanceCount; i++)
	{
		nRandNum -= cbChanceArray[i];
		if (nRandNum < 0)
		{
			cbCurChip = i;
			break;
		}
	}

	nPJStFluc = nPJStFluc % 3 + 1;

	return cbCurChip;
}

//ǰ��4��������
bool CAndroidUserItemSink::IsFourCardTypeNiu(BYTE cbCardData[], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig)
{
	BYTE bTemp[MAX_CARDCOUNT];
	BYTE bSum = 0;
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		bTemp[i] = m_GameLogic.GetNNCardLogicValue(cbCardData[i]);
		bSum += bTemp[i];
	}

	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if ((bSum - bTemp[i]) % 10 == 0)
		{
			return true;
		}
	}

	return false;
}

//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
