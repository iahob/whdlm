#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////

//����ʱ��
#define TIME_LESS						2									//����ʱ��

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
#define IDI_DELAY_TIME					(105)									//��ʱ��ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	m_lTurnMaxScore = 0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	m_nRobotBankStorageMul=0;
	m_lRobotBankGetScore=0;
	m_lRobotBankGetScoreBanker=0;
	ZeroMemory(m_lRobotScoreRange,sizeof(m_lRobotScoreRange));

	m_cbDynamicJoin = FALSE;

	m_bgtConfig = BGT_DESPOT_;
	m_btConfig = BT_FREE_;
	
	m_lFreeConfig[0] = 200;
	m_lFreeConfig[1] = 500;
	m_lFreeConfig[2] = 800;
	m_lFreeConfig[3] = 1100;
	m_lFreeConfig[4] = 1500;

	ZeroMemory(m_lPercentConfig, sizeof(m_lPercentConfig));
	m_wBgtRobNewTurnChairID = INVALID_CHAIR;
	m_lCellScore = 0;

	//�ӿڱ���
	m_pIAndroidUserItem=NULL;
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
	m_lTurnMaxScore = 0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
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
		case IDI_DELAY_TIME:
			{
				//��ʼʱ��
				UINT nElapse= rand() % TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

				return true;
			}

		case IDI_START_GAME:		//��ʼ��ʱ��
			{			
				//����׼��
				m_pIAndroidUserItem->SendUserReady(NULL,0);

				return true;
			}
		case IDI_CALL_BANKER:		//��ׯ��ʱ
			{
				//���ñ���
				CMD_C_CallBanker CallBanker;
				ZeroMemory(&CallBanker, sizeof(CallBanker));

				CallBanker.bBanker = (m_wBgtRobNewTurnChairID != INVALID_CHAIR ? true : rand() % 100 > 50);
				if (CallBanker.bBanker == false)
				{
					CallBanker.cbBankerTimes = 0;
				}
				else
				{
					//������ׯ��1��
					CallBanker.cbBankerTimes = (m_bgtConfig == BGT_FREEBANKER_ ? 1 : rand() % 5 + 1);
				}

				//������Ϣ
				m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_BANKER,&CallBanker, sizeof(CallBanker));

				return true;
			}
		case IDI_USER_ADD_SCORE:	//��ע��ʱ
			{
				CMD_C_AddScore AddScore;
				ZeroMemory(&AddScore, sizeof(AddScore));
				
				ASSERT (m_lTurnMaxScore > 0L);

				LONGLONG lAddScore = 0LL;

				if (m_btConfig == BT_FREE_)
				{
					//�ĳ�4��������ҷ��������Ǳ���
					lAddScore = m_lFreeConfig[rand() % (MAX_CONFIG - 1)] * m_lCellScore;

					if (lAddScore == 0 || lAddScore > m_lTurnMaxScore)
					{
						lAddScore = m_lTurnMaxScore;
					}
				}
				else if (m_btConfig == BT_PENCENT_)
				{
					lAddScore = m_lPercentConfig[rand() % MAX_CONFIG] * m_lTurnMaxScore / 100;

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
		cs.Format(TEXT("�쳣ID=%d"),nTimerID);
		CTraceService::TraceString(cs,TraceLevel_Debug);
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
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_ADD_SCORE:	//�û���ע
		{
			//��Ϣ����
			return OnSubAddScore(pData,wDataSize);
		}
	case SUB_S_PLAYER_EXIT:	//�û�ǿ��
		{
			//��Ϣ����
			return OnSubPlayerExit(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:	//������Ϣ
		{
			//��Ϣ����
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_GAME_END:	//��Ϸ����
		{
			//��Ϣ����
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_OPEN_CARD:	//�û�̯��
		{
			//��Ϣ����
			return OnSubOpenCard(pData,wDataSize);
		}
	case SUB_S_CALL_BANKER:	//�û���ׯ
		{
			//��Ϣ����
			return OnSubCallBanker(pData,wDataSize);
		}
	case SUB_S_CALL_BANKERINFO:	//�û���ׯ��Ϣ
		{
			//��Ϣ����
			return OnSubCallBankerInfo(pData,wDataSize);
		}
	case SUB_S_ADMIN_STORAGE_INFO:
	case SUB_S_RECORD:
		{
			return true;
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
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��Ϣ����
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			
			ReadConfigInformation(&(pStatusFree->CustomAndroid));

			BankOperate(2);

			m_bgtConfig = pStatusFree->bgtConfig;
			m_btConfig = pStatusFree->btConfig;
			m_lCellScore = pStatusFree->lCellScore;

			//��ʼʱ��
			UINT nElapse= rand() % TIME_LESS + (rand() % TIME_USER_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

			return true;
		}
	case GS_TK_CALL:	// ��ׯ״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusCall)) return false;
			CMD_S_StatusCall * pStatusCall=(CMD_S_StatusCall *)pData;
			
			m_cbDynamicJoin = pStatusCall->cbDynamicJoin;
			
			ReadConfigInformation(&(pStatusCall->CustomAndroid));

			BankOperate(2);
			
			m_bgtConfig = pStatusCall->bgtConfig;
			m_btConfig = pStatusCall->btConfig;
			m_lCellScore = pStatusCall->lCellScore;

			ASSERT (m_bgtConfig == BGT_ROB_);

			if (m_cbDynamicJoin == FALSE && pStatusCall->cbCallBankerStatus[m_pIAndroidUserItem->GetChairID()] == FALSE)
			{
				//��ׯʱ��
				UINT nElapse = TIME_LESS + (rand() % TIME_USER_CALL_BANKER);
				m_pIAndroidUserItem->SetGameTimer(IDI_CALL_BANKER, nElapse);
			}

			return true;
		}
	case GS_TK_SCORE:	//��ע״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusScore)) return false;
			CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pData;

			m_cbDynamicJoin = pStatusScore->cbDynamicJoin;

			ReadConfigInformation(&(pStatusScore->CustomAndroid));

			BankOperate(2);
			
			m_bgtConfig = pStatusScore->bgtConfig;
			m_btConfig = pStatusScore->btConfig;
			m_lCellScore = pStatusScore->lCellScore;

			CopyMemory(m_lFreeConfig, pStatusScore->lFreeConfig, sizeof(m_lFreeConfig));
			CopyMemory(m_lPercentConfig, pStatusScore->lPercentConfig, sizeof(m_lPercentConfig));

			//���ñ���
			m_lTurnMaxScore=pStatusScore->lTurnMaxScore;
			WORD wMeChairId = m_pIAndroidUserItem->GetChairID();

			//���ó���
			if (pStatusScore->lTurnMaxScore > 0L && pStatusScore->lTableScore[wMeChairId] == 0L && m_cbDynamicJoin == FALSE
				&& wMeChairId != pStatusScore->wBankerUser)
			{
				//��עʱ��
				UINT nElapse = TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
				m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
			}

			return true;
		}
	case GS_TK_PLAYING:	//��Ϸ״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			m_cbDynamicJoin = pStatusPlay->cbDynamicJoin;

			ReadConfigInformation(&(pStatusPlay->CustomAndroid));

			BankOperate(2);

			m_bgtConfig = pStatusPlay->bgtConfig;
			m_btConfig = pStatusPlay->btConfig;
			m_lCellScore = pStatusPlay->lCellScore;

			CopyMemory(m_lFreeConfig, pStatusPlay->lFreeConfig, sizeof(m_lFreeConfig));
			CopyMemory(m_lPercentConfig, pStatusPlay->lPercentConfig, sizeof(m_lPercentConfig));

			//���ñ���
			m_lTurnMaxScore=pStatusPlay->lTurnMaxScore;
			WORD wMeChiarID=m_pIAndroidUserItem->GetChairID();

			CopyMemory(m_cbHandCardData, pStatusPlay->cbHandCardData[wMeChiarID], MAX_CARDCOUNT);

			//�ؼ�����
			if(pStatusPlay->bOpenCard[wMeChiarID] == false && m_cbDynamicJoin == FALSE)
			{
				//����ʱ��
				UINT nElapse = TIME_LESS + 2 + (rand() % TIME_USER_OPEN_CARD);
				m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse);
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
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//���ñ���
	m_lTurnMaxScore=pGameStart->lTurnMaxScore;
	
	CopyMemory(m_cbHandCardData, pGameStart->cbCardData[m_pIAndroidUserItem->GetChairID()], sizeof(m_cbHandCardData));

	m_bgtConfig = pGameStart->bgtConfig;
	m_btConfig = pGameStart->btConfig;

	CopyMemory(m_lFreeConfig, pGameStart->lFreeConfig, sizeof(m_lFreeConfig));
	CopyMemory(m_lPercentConfig, pGameStart->lPercentConfig, sizeof(m_lPercentConfig));

	//���ó���
	if (pGameStart->lTurnMaxScore > 0 && m_cbDynamicJoin == FALSE && m_pIAndroidUserItem->GetChairID() != pGameStart->wBankerUser)
	{
		//��עʱ��
		UINT nElapse = TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
	}

	return true;
}

//�û���ע
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//�����˲������û���ע
	return true;
}

//�û�ǿ��
bool CAndroidUserItemSink::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
	//�����˲������û�ǿ��
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//��������
	WORD wMeChiarID=m_pIAndroidUserItem->GetChairID();
	CopyMemory(m_cbHandCardData, pSendCard->cbCardData[wMeChiarID], sizeof(m_cbHandCardData));

	//����ʱ��
	if (m_cbDynamicJoin == FALSE)
	{
		UINT nElapse = TIME_LESS + 2 + (rand() % TIME_USER_OPEN_CARD);
		m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse);
	}

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_CALL_BANKER);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	m_pIAndroidUserItem->KillGameTimer(IDI_OPEN_CARD);
	
	BankOperate(2);

	UINT nElapse= rand() % TIME_LESS + (rand() % TIME_USER_START_GAME) +2;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	//�������
	m_lTurnMaxScore = 0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	m_cbDynamicJoin = FALSE;

	return true;
}

//�û�̯��
bool CAndroidUserItemSink::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize!=sizeof(CMD_S_Open_Card)) return false;
	CMD_S_Open_Card * pOpenCard=(CMD_S_Open_Card *)pBuffer;

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

	if(m_cbDynamicJoin == FALSE)
	{
		if ((pCallBanker->wBgtRobNewTurnChairID == INVALID_CHAIR) || (pCallBanker->wBgtRobNewTurnChairID != INVALID_CHAIR && pCallBanker->wBgtRobNewTurnChairID == m_pIAndroidUserItem->GetChairID()))
		{
			//��ׯʱ��
			UINT nElapse = TIME_LESS + (rand() % TIME_USER_CALL_BANKER);
			m_pIAndroidUserItem->SetGameTimer(IDI_CALL_BANKER, nElapse);
		}
	}

	return true;
}

//�û���ׯ��Ϣ
bool CAndroidUserItemSink::OnSubCallBankerInfo(const void * pBuffer, WORD wDataSize)
{
	//�����˲������û���ׯ��Ϣ
	return true;
}

//���в���
void CAndroidUserItemSink::BankOperate(BYTE cbType)
{
	//��������
	LONGLONG lRobotScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();

	//�жϴ�ȡ
	if (lRobotScore > m_lRobotScoreRange[1])
	{
		LONGLONG lSaveScore = 0L;

		lSaveScore = LONGLONG(lRobotScore*m_nRobotBankStorageMul / 100);
		if (lSaveScore > lRobotScore)  lSaveScore = lRobotScore;

		if (lSaveScore > 0)
			m_pIAndroidUserItem->PerformSaveScore(lSaveScore);

		LONGLONG lRobotNewScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();

		CString strdebug;
		strdebug.Format(TEXT("������USERID = ��%d��, ��ʼ����ʱ���ϵĽ�ҡ�%I64d��\n"), m_pIAndroidUserItem->GetUserID(), m_pIAndroidUserItem->GetMeUserItem()->GetUserScore());
		WriteInfo(strdebug);

		CString strInfo;
		strInfo.Format(TEXT("[%s] ִ�д����ǰ���(%I64d)��������(%I64d)"), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(), lRobotScore, lRobotNewScore);

		WriteInfo(strInfo);
	}
	else if (lRobotScore < m_lRobotScoreRange[0])
	{
		CString strInfo;
		LONGLONG lRobotNewScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();

		SCORE lScore = (lRobotNewScore < 0 ? ((-lRobotNewScore) + rand() % m_lRobotBankGetScoreBanker + m_lRobotBankGetScore) : (rand() % m_lRobotBankGetScoreBanker + m_lRobotBankGetScore));

		if (lScore > 0)
			m_pIAndroidUserItem->PerformTakeScore(lScore);

		CString strdebug;
		strdebug.Format(TEXT("������USERID = ��%d��, ��ʼȡ���ʱ���ϵĽ�ҡ�%I64d��\n"), m_pIAndroidUserItem->GetUserID(), m_pIAndroidUserItem->GetMeUserItem()->GetUserScore());
		WriteInfo(strdebug);

		strInfo.Format(TEXT("[%s] ִ��ȡ�ȡ��ǰ���(%I64d)��ȡ�����(%I64d)"), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(), lRobotScore, lRobotNewScore);

		WriteInfo(strInfo);
	}
}

//д��־�ļ�
void CAndroidUserItemSink::WriteInfo(LPCTSTR pszString)
{
	return;

	//������������
	char* old_locale = _strdup( setlocale(LC_CTYPE,NULL) );
	setlocale( LC_CTYPE, "chs" );

	CStdioFile myFile;
	CString strFileName = TEXT("������ţţ�����˴�ȡ���¼.txt");
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
	if ( bOpen )
	{	
		myFile.SeekToEnd();
		myFile.WriteString( pszString );
		myFile.Flush();
		myFile.Close();
	}

	//��ԭ�����趨
	setlocale( LC_CTYPE, old_locale );
	free( old_locale );
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


//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
