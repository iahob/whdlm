#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//����ʱ��
#define TIME_LESS						2									//����ʱ��

//��Ϸʱ��
#define TIME_USER_CALL_BANKER			2									//��ׯʱ��
#define TIME_USER_START_GAME			3									//��ʼʱ��
#define TIME_USER_ADD_SCORE				3									//��עʱ��
#define TIME_USER_OPEN_CARD				12									//̯��ʱ��

#define TIME_CHECK_BANKER				30									//�������

//��Ϸʱ��
#define IDI_START_GAME					(100)									//��ʼ��ʱ��
#define IDI_USER_ADD_SCORE				(102)									//��ע��ʱ��
#define IDI_GET_CARD					(103)									//Ҫ�ƶ�ʱ��
#define IDI_STOP_CARD					(104)									//ͣ�ƶ�ʱ��
#define IDI_SPLIT_CARD					(105)									//���ƶ�ʱ��
#define IDI_DOUBLE_CARD					(106)									//�ӱ���ʱ��


#define IDI_CHECK_BANKER_OPERATE        (110)									//��鶨ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	m_lTurnMaxScore = 0;
	m_lCellScore = 0;
	m_wBankerUser = INVALID_CHAIR;
	ZeroMemory(m_HandCardData,sizeof(m_HandCardData));
	ZeroMemory( m_bStopCard,sizeof(m_bStopCard) );
	ZeroMemory( m_bInsureCard,sizeof(m_bInsureCard) );
	ZeroMemory( m_bDoubleCard,sizeof(m_bDoubleCard) );
	ZeroMemory( m_cbCardCount,sizeof(m_cbCardCount) );

	m_nRobotBankStorageMul=0;
	m_lRobotBankGetScore=0;
	m_lRobotBankGetScoreBanker=0;
	ZeroMemory(m_lRobotScoreRange,sizeof(m_lRobotScoreRange));
	m_bWin=false;
	
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
	ZeroMemory(m_HandCardData,sizeof(m_HandCardData));
	ZeroMemory( m_bStopCard,sizeof(m_bStopCard) );
	ZeroMemory( m_bInsureCard,sizeof(m_bInsureCard) );
	ZeroMemory( m_bDoubleCard,sizeof(m_bDoubleCard) );
	ZeroMemory( m_cbCardCount,sizeof(m_cbCardCount) );
	m_wBankerUser = INVALID_CHAIR;

	//�������
	//UINT nElapse=TIME_CHECK_BANKER+rand()%100;
	//m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER_OPERATE,nElapse);

	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	try
	{
		switch (nTimerID)
		{		
		case IDI_CHECK_BANKER_OPERATE://�������
			{

// 				IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
// 				if (NULL == pUserItem) return true;
// 				if(pUserItem->GetUserStatus()<US_SIT)
// 				{
// 					//ReadConfigInformation();
// 					//BankOperate(1);
// 				}
// 				//�������
// 				UINT nElapse=TIME_CHECK_BANKER+rand()%100;
// 				m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER_OPERATE,nElapse);

				return true;
			}
		case IDI_START_GAME:		//��ʼ��ʱ��
			{
				//����׼��
				m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);

				m_pIAndroidUserItem->SendUserReady(NULL,0);

				return true;
			}		
		case IDI_USER_ADD_SCORE:	//��ע��ʱ
			{
				m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
				//������Ϣ
				CMD_C_AddScore AddScore;
				//�������
				AddScore.lScore = ((rand() % 4) + 1)*m_lCellScore;
				if (AddScore.lScore <= 0)
				{
					AddScore.lScore = m_lCellScore;
				}
				
				m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(CMD_C_AddScore));

				return true;	
			}
		case IDI_STOP_CARD:			//ͣ�ƶ�ʱ
			{
				m_pIAndroidUserItem->KillGameTimer(IDI_STOP_CARD);
				m_pIAndroidUserItem->SendSocketData(SUB_C_STOP_CARD, NULL, 0);
				return true;	
			}
		case IDI_DOUBLE_CARD:		//�ӱ���ʱ
			{
				m_pIAndroidUserItem->KillGameTimer(IDI_DOUBLE_CARD);
				m_pIAndroidUserItem->SendSocketData(SUB_C_DOUBLE_SCORE, NULL, 0);
				return true;
			}
		case IDI_SPLIT_CARD:		//���ƶ�ʱ
			{
				m_pIAndroidUserItem->KillGameTimer(IDI_SPLIT_CARD);
				m_pIAndroidUserItem->SendSocketData(SUB_C_SPLIT_CARD, NULL, 0);
				return true;
			}
		case IDI_GET_CARD:			//Ҫ�ƶ�ʱ
			{
				m_pIAndroidUserItem->KillGameTimer(IDI_GET_CARD);
				m_pIAndroidUserItem->SendSocketData(SUB_C_GET_CARD, NULL, 0);

// 				CStringA lyt;
// 				lyt.Format("lytlog::������ ��ʱ Ҫ�� OnEventTimer()  wChairID = %d m_wBankerUser = %d", m_pIAndroidUserItem->GetChairID(), m_wBankerUser);
// 				OutputDebugStringA(lyt); 
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
	case SUB_S_ADD_SCORE:	//��ע
		{
			return OnSubAddScore( pData,wDataSize );
		}
	case SUB_S_SEND_CARD:	//������Ϣ
		{
			//��Ϣ����
			return OnSubSendCard(pData, wDataSize);
		}
	case SUB_S_STOP_CARD:	//ͣ��
		{			
			//��Ϣ����
			return OnSubStopCard(pData,wDataSize);
		}
	case SUB_S_DOUBLE_SCORE://�ӱ�
		{			
			//��Ϣ����
			return OnSubDoubleCard(pData,wDataSize);
		}
	case SUB_S_SPLIT_CARD:	//����
		{			
			//��Ϣ����
			return OnSubSplitCard(pData,wDataSize);
		}
	case SUB_S_INSURE:		//����
		{
			//��Ϣ����
			return OnSubInsureCard(pData,wDataSize);
		}
	case SUB_S_GET_CARD:	//Ҫ��
		{			
			//��Ϣ����
			return OnSubGetCard(pData,wDataSize);
		}	
	case SUB_S_GAME_END:	//��Ϸ����
		{
			//��Ϣ����
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_CHEAT_CARD:
		{
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
			CString strGameLog;
			strGameLog.Format(TEXT("  AI ����״̬()  000  \n"));
			m_pIAndroidUserItem->SendLogData(strGameLog);
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��Ϣ����
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//��ȡ����
			ReadConfigInformation(&(pStatusFree->CustomAndroid));
			BankOperate(2);

			//��ʼʱ��
			UINT nElapse = rand() % TIME_LESS + TIME_LESS;
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

			strGameLog.Format(TEXT("  AI ����״̬()  111  \n"));
			m_pIAndroidUserItem->SendLogData(strGameLog);

			return true;
		}
	case GAME_SCENE_ADD_SCORE:	//��Ϸ״̬
		{
			CString strGameLog;
			strGameLog.Format(TEXT("  AI ��ע״̬()  000  \n"));
			m_pIAndroidUserItem->SendLogData(strGameLog);
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusAddScore)) return false;
			CMD_S_StatusAddScore * pStatusScore=(CMD_S_StatusAddScore *)pData;

			//��ȡ����
			ReadConfigInformation(&(pStatusScore->CustomAndroid));

			BankOperate(2);	

			//���ñ���
			m_lTurnMaxScore=pStatusScore->lMaxScore;
			WORD wMeChairId = m_pIAndroidUserItem->GetChairID();

			//���ó���
			if (pStatusScore->lMaxScore>0L && pStatusScore->lTableScore[wMeChairId]==0L)
			{
				//��עʱ��
				UINT nElapse = rand() % TIME_LESS + TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
			}

			strGameLog.Format(TEXT("  AI ��ע״̬()  111  \n"));
			m_pIAndroidUserItem->SendLogData(strGameLog);

			return true;
		}
	case GAME_SCENE_GET_CARD:
		{
			CString strGameLog;
			strGameLog.Format(TEXT("  AI ��Ϸ״̬()  000  \n"));
			m_pIAndroidUserItem->SendLogData(strGameLog);
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusGetCard)) return false;
			CMD_S_StatusGetCard * pStatusGetCard=(CMD_S_StatusGetCard *)pData;

			//��ȡ����
			ReadConfigInformation(&(pStatusGetCard->CustomAndroid));

			BankOperate(2);	

			WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
			if (pStatusGetCard->wCurrentUser != NULL && wMeChairId == pStatusGetCard->wCurrentUser)
				
			{
				//���ö�ʱ��
// 				UINT nElapse = rand() % TIME_LESS + TIME_LESS;
// 				m_pIAndroidUserItem->SetGameTimer(IDI_GET_CARD, nElapse);

				if (pStatusGetCard->bStopCard[wMeChairId * 2] == false)
				{
// 					BYTE cbCardType = 0;
// 					cbCardType = m_GameLogic.GetCardType(m_HandCardData[0], m_cbCardCount[0], false);
// 					BYTE cbLastCardValue = 0;
// 					for (int i = 1; i < m_cbCardCount[0]; i++)
// 					{
// 						cbLastCardValue += m_GameLogic.GetCardLogicValue(m_HandCardData[0][i]);
// 					}
// 					//��������
// 					AnalyseCardOperate(cbCardType, cbLastCardValue);

					//���ö�ʱ��
					UINT nElapse = rand() % TIME_LESS + TIME_LESS;
					m_pIAndroidUserItem->SetGameTimer(IDI_STOP_CARD, nElapse);
				}
			}

			strGameLog.Format(TEXT("  AI ��Ϸ״̬()  111  \n"));
			m_pIAndroidUserItem->SendLogData(strGameLog);

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

//Ҫ��
bool CAndroidUserItemSink::OnSubGetCard( const void *pBuffer, WORD wDataSize )
{
	CString strGameLog;
	strGameLog.Format(TEXT("  AI OnSubGetCard()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);
	//Ч������
	if (wDataSize!=sizeof(CMD_S_GetCard)) return false;
	CMD_S_GetCard * pGetCard=(CMD_S_GetCard *)pBuffer;	

	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	if( wMeChairId == pGetCard->wGetCardUser )
	{
		WORD wIndex = m_bStopCard[wMeChairId*2]?1:0;
		m_HandCardData[wIndex][m_cbCardCount[wIndex]++] = pGetCard->cbCardData;		
		
		bool bSplitCard = m_cbCardCount[1] > 0 ? true : false;

		BYTE cbCardType = m_GameLogic.GetCardType( m_HandCardData[wIndex],m_cbCardCount[wIndex],bSplitCard);		

		BYTE cbLastCardValue = 0;
		for (int i = 1; i < m_cbCardCount[wIndex]; i++)
		{
			cbLastCardValue += m_GameLogic.GetCardLogicValue(m_HandCardData[wIndex][i]);
		}
		//ׯ��17�����±���Ҫ��
		if (wMeChairId == m_wBankerUser && cbCardType <= 17 && CT_BAOPAI != cbCardType)
		{
			//��עʱ��
			UINT nElapse = rand() % TIME_LESS + TIME_LESS;
			m_pIAndroidUserItem->SetGameTimer(IDI_GET_CARD, nElapse);

// 			CStringA lyt;
// 			lyt.Format("lytlog::������OnSubGetCard() wMeChairId = %d m_wBankerUser = %d cbCardType = %d ", wMeChairId, m_wBankerUser, cbCardType);
// 			OutputDebugStringA(lyt);
		}
		else
		{
			//��������
			AnalyseCardOperate(cbCardType, cbLastCardValue);
		}
	}

	strGameLog.Format(TEXT("  AI OnSubGetCard()  111  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	return true;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	CString strGameLog;
	strGameLog.Format(TEXT("  AI OnSubGameStart()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);
	//Ч������
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	if (NULL == m_pIAndroidUserItem) return true;
	//ɾ����ʱ��	
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	m_pIAndroidUserItem->KillGameTimer(IDI_STOP_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_GET_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_SPLIT_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_DOUBLE_CARD);

	//���ñ���
	m_lTurnMaxScore=pGameStart->lMaxScore;
	m_lCellScore = pGameStart->lCellScore;
	m_wBankerUser = pGameStart->wBankerUser;

	//���ó���
	if (m_wBankerUser != m_pIAndroidUserItem->GetChairID())
	{
		//��עʱ��
		UINT nElapse = rand() % TIME_LESS + TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
	}

	strGameLog.Format(TEXT("  AI OnSubGameStart()  111  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	return true;
}

//�û���ע
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;

	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	CString strGameLog;
	strGameLog.Format(TEXT("  AI OnSubSendCard()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	//Ч������
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//���ñ���
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	m_cbCardCount[0] = 2;
	CopyMemory( m_HandCardData[0],pSendCard->cbHandCardData[wMeChairId],sizeof(BYTE)*2 );
	m_bWin = pSendCard->bWin;

	if (m_wBankerUser != wMeChairId && pSendCard->wCurrentUser == wMeChairId)
	{
// 		if (m_GameLogic.GetCardValue(m_HandCardData[0][1]) == m_GameLogic.GetCardValue(m_HandCardData[0][0]))
// 		{
// 			//��עʱ��
// 			UINT nElapse=rand()%(4)+TIME_LESS;
// 			m_pIAndroidUserItem->SetGameTimer(IDI_SPLIT_CARD,nElapse);
// 		}
// 		else
		{
			BYTE cbCardType = 0;
			cbCardType = m_GameLogic.GetCardType( m_HandCardData[0],m_cbCardCount[0],false );
			BYTE cbLastCardValue = 0;
			for (int i = 1; i < m_cbCardCount[0]; i++)
			{
				cbLastCardValue += m_GameLogic.GetCardLogicValue(m_HandCardData[0][i]);
			}
			//��������
			AnalyseCardOperate(cbCardType, cbLastCardValue);
		}
	}

	strGameLog.Format(TEXT("  AI OnSubSendCard()  111  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	return true;
}

//ͣ����Ϣ
bool CAndroidUserItemSink::OnSubStopCard(const void * pBuffer, WORD wDataSize)
{
	CString strGameLog;
	strGameLog.Format(TEXT("  AI OnSubStopCard()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	//Ч��
	if( wDataSize != sizeof(CMD_S_StopCard) ) return false;
	CMD_S_StopCard *pStopCard = (CMD_S_StopCard *)pBuffer;

	if (pStopCard->wStopCardUser >= GAME_PLAYER) return true;
	
	//����ͣ�Ʊ���
	WORD wOperIndex = pStopCard->wStopCardUser*2;
	if( m_bStopCard[wOperIndex] )
		m_bStopCard[++wOperIndex] = TRUE;
	else m_bStopCard[wOperIndex] = TRUE;

	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
// 	if (pStopCard->wStopCardUser == wMeChairId )
// 	{
// 		if (wOperIndex%2 == 0 && m_cbCardCount[1] > 0 )
// 		{
// 			BYTE cbCardType = m_GameLogic.GetCardType( m_HandCardData[1],m_cbCardCount[1],true);		
// 
// 			BYTE cbLastCardValue = 0;
// 			for (int i = 1; i < m_cbCardCount[1]; i++)
// 			{
// 				cbLastCardValue += m_GameLogic.GetCardLogicValue(m_HandCardData[1][i]);
// 			}
// 			//��������
// 			AnalyseCardOperate(cbCardType, cbLastCardValue);
// 		}		
// 	}

	//�Ƿ��ֵ�ׯ�Ҳ���
	if (pStopCard->wCurrentUser == wMeChairId && pStopCard->wStopCardUser != m_wBankerUser)
	{		
		BYTE cbCardType = 0;
		cbCardType = m_GameLogic.GetCardType( m_HandCardData[0],m_cbCardCount[0],false );

		BYTE cbLastCardValue = 0;
		for (int i = 1; i < m_cbCardCount[0]; i++)
		{
			cbLastCardValue += m_GameLogic.GetCardLogicValue(m_HandCardData[0][i]);
		}
		//��������
		AnalyseCardOperate(cbCardType, cbLastCardValue);				
	}

	strGameLog.Format(TEXT("  AI OnSubStopCard()  111  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	return true;
}

//�ӱ���Ϣ
bool CAndroidUserItemSink::OnSubDoubleCard(const void * pBuffer, WORD wDataSize)
{
	CString strGameLog;
	strGameLog.Format(TEXT("  AI OnSubDoubleCard()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	//Ч��
	if( wDataSize != sizeof(CMD_S_DoubleScore) ) return false;
	CMD_S_DoubleScore *pDoubleScore = (CMD_S_DoubleScore *)pBuffer;

	//�����Լ��ӱ�
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	if( wMeChairId == pDoubleScore->wDoubleScoreUser )
	{
		WORD wIndex = m_bStopCard[wMeChairId*2]?1:0;
		m_HandCardData[wIndex][m_cbCardCount[wIndex]++] = pDoubleScore->cbCardData;
	}
	//���üӱ�
	if( wMeChairId == pDoubleScore->wDoubleScoreUser )
	{
		if( !m_bDoubleCard[0] ) m_bDoubleCard[0] = TRUE;
		else m_bDoubleCard[1] = TRUE;
	}

	strGameLog.Format(TEXT("  AI OnSubDoubleCard()  111  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{

	CString strGameLog;
	strGameLog.Format(TEXT("  AI OnSubGameEnd()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	//Ч�����
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//ɾ����ʱ��	
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	m_pIAndroidUserItem->KillGameTimer(IDI_STOP_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_GET_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_SPLIT_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_DOUBLE_CARD);	
	
	//��ʼʱ��
	//if(m_pIAndroidUserItem->GetChairID() != INVALID_CHAIR)
	{
		UINT nElapse = rand() % TIME_LESS + TIME_LESS * 2;
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
	}

	//�������
	m_lTurnMaxScore = 0;	
	ZeroMemory(m_HandCardData,sizeof(m_HandCardData));
	ZeroMemory( m_bStopCard,sizeof(m_bStopCard) );
	ZeroMemory( m_bInsureCard,sizeof(m_bInsureCard) );
	ZeroMemory( m_bDoubleCard,sizeof(m_bDoubleCard) );
	ZeroMemory( m_cbCardCount,sizeof(m_cbCardCount) );

	strGameLog.Format(TEXT("  AI OnSubGameEnd()  111  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnSubSplitCard(const void * pBuffer, WORD wDataSize)
{
	CString strGameLog;
	strGameLog.Format(TEXT("  AI OnSubSplitCard()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	//Ч��
	if( wDataSize != sizeof(CMD_S_SplitCard) ) return false;
	CMD_S_SplitCard *pSplitCard = (CMD_S_SplitCard *)pBuffer;

	//�����Լ�����
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	if( pSplitCard->wSplitUser == wMeChairId )
	{		
		m_HandCardData[1][1] = m_HandCardData[0][1];
		m_HandCardData[0][0] = pSplitCard->cbCardData[0];
		m_HandCardData[1][0] = pSplitCard->cbCardData[1];
		m_cbCardCount[0] = m_cbCardCount[1] = 2;			

		BYTE cbCardType = m_GameLogic.GetCardType( m_HandCardData[0],m_cbCardCount[0],false);		

		//��������
		AnalyseCardOperate(cbCardType, m_GameLogic.GetCardLogicValue(m_HandCardData[0][1]));
	}

	strGameLog.Format(TEXT("  AI OnSubSplitCard()  111  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	return true;
}

//��������
bool CAndroidUserItemSink::AnalyseCardOperate(BYTE cbCardType, BYTE cbLastVaule)
{
	CString strGameLog;
	strGameLog.Format(TEXT("  AI AnalyseCardOperate()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	//ɾ����ʱ��	
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	m_pIAndroidUserItem->KillGameTimer(IDI_STOP_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_GET_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_SPLIT_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_DOUBLE_CARD);

	if (cbCardType == CT_BAOPAI || cbCardType >= CT_BJ - 1)
	{
		//��עʱ��
		UINT nElapse = rand() % TIME_LESS + TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_STOP_CARD, nElapse);
		return true;
	}
	else
	{
// 		if (m_bWin)
// 		{
// 			if (cbLastVaule < 11)
// 			{
// 				//��עʱ��
// 				UINT nElapse=rand()%(4)+TIME_LESS;
// 				m_pIAndroidUserItem->SetGameTimer(IDI_GET_CARD,nElapse);
// 				return true;
// 			}
// 			else
// 			{
// 				//��עʱ��
// 				UINT nElapse=rand()%(4)+TIME_LESS;
// 				m_pIAndroidUserItem->SetGameTimer(IDI_STOP_CARD,nElapse);
// 				return true;
// 			}
// 		}
// 		else
		{
			if (cbCardType < 15)
			{
				//��עʱ��
				UINT nElapse = rand() % TIME_LESS + TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_GET_CARD,nElapse);
				return true;
			}
			else if(cbCardType > 16)
			{				
				//��עʱ��
				UINT nElapse = rand() % TIME_LESS + TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_STOP_CARD, nElapse);
				return true;
			}
			else
			{
				int nRand = rand()%2;
				if (nRand == 0)
				{
					//��עʱ��
					UINT nElapse = rand() % TIME_LESS+TIME_LESS;
					m_pIAndroidUserItem->SetGameTimer(IDI_STOP_CARD, nElapse);
					return true;
				}
				else
				{
					//��עʱ��
					UINT nElapse = rand() % TIME_LESS+TIME_LESS;
					m_pIAndroidUserItem->SetGameTimer(IDI_GET_CARD,nElapse);
					return true;
				}
			}
		}
	}
}

//������Ϣ
bool CAndroidUserItemSink::OnSubInsureCard(const void * pBuffer, WORD wDataSize)
{
	CString strGameLog;
	strGameLog.Format(TEXT("  AI OnSubInsureCard()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	//Ч��
	if( wDataSize != sizeof(CMD_S_Insure) ) return false;
	CMD_S_Insure *pInsureCard = (CMD_S_Insure *)pBuffer;

	//���ñ���
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	if( wMeChairId == pInsureCard->wInsureUser )
	{
		if( !m_bStopCard[wMeChairId*2] ) m_bInsureCard[0] = TRUE;
		else m_bInsureCard[1] = TRUE;
	}

	strGameLog.Format(TEXT("  AI OnSubInsureCard()  111  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	return true;
}

//���в���
void CAndroidUserItemSink::BankOperate(BYTE cbType)
{
	CString strGameLog;
	strGameLog.Format(TEXT("  AI BankOperate()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	if(pUserItem->GetUserStatus()>=US_SIT)
	{
		if(cbType==1)
		{
			CString strInfo;
			strInfo.Format(TEXT("������״̬���ԣ���ִ�д�ȡ��"));
			CTraceService::TraceString(strInfo,TraceLevel_Normal);
			return;			
		}
	}
	
	//��������
	LONGLONG lRobotScore = pUserItem->GetUserScore();

	//CString strInfo;
	//strInfo.Format(TEXT("������Ϣ��ȡ������(%I64d)���������(%I64d),��ϷAI����(%I64d)"),m_lRobotScoreRange[0],m_lRobotScoreRange[1],lRobotScore);
	//if(cbType==1) 
	//NcaTextOut(strInfo);

	{
		CString strInfo;
		strInfo.Format(TEXT("[%s] ����(%I64d)"),pUserItem->GetNickName(),lRobotScore);

		if (lRobotScore > m_lRobotScoreRange[1])
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����������(%I64d)"),m_lRobotScoreRange[1]);
			strInfo+=strInfo1;
			//CTraceService::TraceString(strInfo,TraceLevel_Normal);
		}
		else if (lRobotScore < m_lRobotScoreRange[0])
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����ȡ������(%I64d)"),m_lRobotScoreRange[0]);
			strInfo+=strInfo1;
			//CTraceService::TraceString(strInfo,TraceLevel_Normal);
		}

		//�жϴ�ȡ
		if (lRobotScore > m_lRobotScoreRange[1])
		{			
			LONGLONG lSaveScore=0L;

			lSaveScore = LONGLONG(lRobotScore*m_nRobotBankStorageMul/100);
			if (lSaveScore > lRobotScore)  lSaveScore = lRobotScore;

			if (lSaveScore > 0)
				m_pIAndroidUserItem->PerformSaveScore(lSaveScore);

			LONGLONG lRobotNewScore = pUserItem->GetUserScore();

			CString strInfo;
			strInfo.Format(TEXT("[%s] ִ�д����ǰ���(%I64d)��������(%I64d)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			//CTraceService::TraceString(strInfo,TraceLevel_Normal);

		}
		else if (lRobotScore < m_lRobotScoreRange[0])
		{

			CString strInfo;
			//strInfo.Format(TEXT("������Ϣ��ȡ����Сֵ(%I64d)��ȡ�����ֵ(%I64d)"),m_lRobotBankGetScore,m_lRobotBankGetScoreBanker);

			//if(cbType==1) 
			//	NcaTextOut(strInfo);
			
			SCORE lScore=rand()%m_lRobotBankGetScoreBanker+m_lRobotBankGetScore;
			if (lScore > 0)
				m_pIAndroidUserItem->PerformTakeScore(lScore);

			LONGLONG lRobotNewScore = pUserItem->GetUserScore();

			//CString strInfo;
			strInfo.Format(TEXT("[%s] ִ��ȡ�ȡ��ǰ���(%I64d)��ȡ�����(%I64d)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			//CTraceService::TraceString(strInfo,TraceLevel_Normal);
					
		}
	}

	strGameLog.Format(TEXT("  AI BankOperate()  111  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);
}

//��ȡ����
void CAndroidUserItemSink::ReadConfigInformation(tagCustomAndroid *pCustomAndroid)
{
	CString strGameLog;
	strGameLog.Format(TEXT("  AI ReadConfigInformation()  000  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);

	m_lRobotScoreRange[0] = pCustomAndroid->lRobotScoreMin;
	m_lRobotScoreRange[1] = pCustomAndroid->lRobotScoreMax;

	if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])	
		m_lRobotScoreRange[1] = m_lRobotScoreRange[0];

	m_lRobotBankGetScore = pCustomAndroid->lRobotBankGet;
	m_lRobotBankGetScoreBanker = pCustomAndroid->lRobotBankGetBanker;
	m_nRobotBankStorageMul = (int)pCustomAndroid->lRobotBankStoMul;

	if (m_nRobotBankStorageMul<0||m_nRobotBankStorageMul>100) 
		m_nRobotBankStorageMul =20;

	strGameLog.Format(TEXT("  AI ReadConfigInformation()  111  \n"));
	m_pIAndroidUserItem->SendLogData(strGameLog);
}


//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
