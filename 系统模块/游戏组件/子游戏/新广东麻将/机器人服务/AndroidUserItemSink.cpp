#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "AndroidAI.h"

//////////////////////////////////////////////////////////////////////////

//����ʱ��

#ifdef _DEBUG
#define TIME_LESS					1									//����ʱ��
#define TIME_START_GAME				1									//��ʼʱ��
#define TIME_OPERATE_CARD			1									//������ʱ��
#else
#define TIME_LESS					2									//����ʱ��
#define TIME_START_GAME				10									//��ʼʱ��
#define TIME_OPERATE_CARD			8									//������ʱ��
#endif

//��Ϸʱ��
#define IDI_START_GAME				(101)			//��ʼʱ��
#define IDI_OPERATE_CARD			(102)			//������ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//��������
	m_cbMagicMode = 0;
	m_cbMaCount = 0;
	m_bTail = false;
	ZeroMemory(m_bTing, sizeof(m_bTing));
	m_cbSendCard = 0;

	//���Ʊ���
	m_wOutCardUser = INVALID_CHAIR;
	m_cbOutCardData = 0;
	m_cbEnjoinOutCardCount = 0;
	ZeroMemory(m_cbEnjoinOutCard, sizeof(m_cbEnjoinOutCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));

	//���Ʊ���
	ZeroMemory(m_cbHuaCardCount, sizeof(m_cbHuaCardCount));
	ZeroMemory(m_cbHuaCardData, sizeof(m_cbHuaCardData));

	//����˿�
	ZeroMemory(m_WeaveItemArray, sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

	//��Ϸ����
	m_wBankerUser = INVALID_CHAIR;	
	m_wCurrentUser = INVALID_CHAIR;
	memset(m_cbMagicIndex,MAX_INDEX,sizeof(m_cbMagicIndex));
	m_cbActionMask = 0;
	m_cbActionCard = 0;
	m_bSelectMode = false;

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
	//��������
	m_bTail = false;
	ZeroMemory(m_bTing,sizeof(m_bTing));
	m_cbSendCard = 0;

	//���Ʊ���
	m_wOutCardUser = INVALID_CHAIR;
	m_cbOutCardData = 0;
	m_cbEnjoinOutCardCount = 0;
	ZeroMemory(m_cbEnjoinOutCard,sizeof(m_cbEnjoinOutCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));

	//����˿�
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//��Ϸ����
	m_wBankerUser = INVALID_CHAIR;	
	m_wCurrentUser = INVALID_CHAIR;
	memset(m_cbMagicIndex,MAX_INDEX,sizeof(m_cbMagicIndex));
	m_cbActionMask = 0;
	m_cbActionCard = 0;
	m_bSelectMode = false;

	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//��ʼ��Ϸ
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);

			//��ʼ�ж�
			if( m_pIAndroidUserItem->GetGameStatus()==GAME_SCENE_FREE )
			{
				m_pIAndroidUserItem->SendUserReady(NULL,0);
			}

			return true;
		}
	case IDI_OPERATE_CARD:		//������ʱ��
		{
			//�Զ�����
			return AutomatismOutCard();
		}
	}

	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//�û�����
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:		//������Ϣ
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_OPERATE_NOTIFY:	//������ʾ
		{
			return OnSubOperateNotify(pData,wDataSize);
		}
	case SUB_S_OPERATE_RESULT:	//�������
		{
			return OnSubOperateResult(pData,wDataSize);
		}
	case SUB_S_LISTEN_CARD:		//�û�����
		{
			return OnSubListenCard(pData,wDataSize);
		}
	case SUB_S_GAME_CONCLUDE:	//��Ϸ����
		{
			return OnSubGameConclude(pData,wDataSize);
		}
	case SUB_S_TRUSTEE:			//�û��й�
		{
			return OnSubTrustee(pData,wDataSize);
		}
	case SUB_S_SET_BASESCORE:	//���û���
		{
			return true;
		}
	case SUB_S_TING_DATA:
		{
			return OnSubTingData(pData,wDataSize);;
		}
	case SUB_S_HU_DATA:
		{
			return OnSubHuData(pData,wDataSize);;
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
	case GAME_SCENE_FREE:	//����״̬
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_S_StatusFree));
			if (wDataSize != sizeof(CMD_S_StatusFree)) return false;

			//��������
			CMD_S_StatusFree * pStatusFree = (CMD_S_StatusFree *)pData;

			//ʱ�䶨��
			m_cbTimeOutCard = pStatusFree->cbTimeOutCard;
			m_cbTimeOperateCard = pStatusFree->cbTimeOperateCard;		
			m_cbTimeStartGame = pStatusFree->cbTimeStartGame;
			m_cbTimeWaitEnd = pStatusFree->cbTimeWaitEnd;

			m_cbAllCardCount = pStatusFree->cbAllCardCount;
			m_cbMagicMode = pStatusFree->cbMagicMode;
			m_cbMaCount = pStatusFree->cbMaCount;
			int nRandTime = 1 + (rand() % __max(1, m_cbTimeStartGame / 2));
#ifdef _DEBUG
			nRandTime = 1;
#endif // _DEBUG
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nRandTime);

			return true;
		}
	case GAME_SCENE_PLAY:	//��Ϸ״̬
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_S_StatusPlay));
			if (wDataSize != sizeof(CMD_S_StatusPlay)) return false;

			WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
			CMD_S_StatusPlay * pStatusPlay = (CMD_S_StatusPlay *)pData;

			//ʱ�䶨��
			m_cbTimeOutCard = pStatusPlay->cbTimeOutCard;
			m_cbTimeOperateCard = pStatusPlay->cbTimeOperateCard;		
			m_cbTimeStartGame = pStatusPlay->cbTimeStartGame;
			m_cbTimeWaitEnd = pStatusPlay->cbTimeWaitEnd;

			//���ñ���
			CopyMemory(m_cbMagicIndex,pStatusPlay->cbMagicIndex,sizeof(m_cbMagicIndex));
			m_wBankerUser = pStatusPlay->wBankerUser;
			m_wCurrentUser = pStatusPlay->wCurrentUser;
			m_wOutCardUser = pStatusPlay->wOutCardUser;
			m_cbOutCardData = pStatusPlay->cbOutCardData;
			m_cbLeftCardCount = pStatusPlay->cbLeftCardCount;
			m_cbAllCardCount = pStatusPlay->cbAllCardCount;
			m_cbMagicMode = pStatusPlay->cbMagicMode;
			m_cbMaCount = pStatusPlay->cbMaCount;

			//�˿˱���
			CopyMemory(m_cbWeaveItemCount, pStatusPlay->cbWeaveItemCount, sizeof(m_cbWeaveItemCount));
			CopyMemory(m_WeaveItemArray, pStatusPlay->WeaveItemArray, sizeof(m_WeaveItemArray));
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData, pStatusPlay->cbCardCount[wMeChairID], m_cbCardIndex);
			CopyMemory(m_cbDiscardCount, pStatusPlay->cbDiscardCount, sizeof(m_cbDiscardCount));
			CopyMemory(m_cbDiscardCard, pStatusPlay->cbDiscardCard, sizeof(m_cbDiscardCard));


			//��������
			if (((pStatusPlay->cbActionMask != WIK_NULL) || m_wCurrentUser == wMeChairID))
			{
				//��ȡ����
				m_cbActionMask = pStatusPlay->cbActionMask;
				m_cbActionCard = pStatusPlay->cbActionCard;

				//����ʱ��
				int nRandTime = 1 + (rand() % __max(1, m_cbTimeOperateCard / 2));
#ifdef _DEBUG
				nRandTime = 1;
#endif // _DEBUG
				m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime);

				//���ƴ���
				if(m_bTing[wMeChairID])
				{				
					if (m_wCurrentUser == INVALID_CHAIR)
					{
						//��������
						OnOperateCard( WIK_NULL,0 );
					}
					else
					{
						//�Զ�����
						AutomatismOutCard();
					}		
				}		
			}

			return true;
		}
	}

	//�������
	ASSERT(FALSE);
	return true;
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
bool CAndroidUserItemSink::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_S_GameStart));
	if (wDataSize != sizeof(CMD_S_GameStart)) return false;

	//��������
	m_bTail = false;
	ZeroMemory(m_bTing, sizeof(m_bTing));
	ZeroMemory(m_cbHuaCardCount, sizeof(m_cbHuaCardCount));
	ZeroMemory(m_cbHuaCardData, sizeof(m_cbHuaCardData));

	//���Ʊ���
	m_wOutCardUser = INVALID_CHAIR;
	m_cbOutCardData = 0;
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));

	//����˿�
	ZeroMemory(m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));
	ZeroMemory(m_WeaveItemArray, sizeof(m_WeaveItemArray));
	
	//��Ϸ����
	m_wCurrentUser = INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;
	m_bSelectMode = false;

	//��������
	CMD_S_GameStart * pGameStart = (CMD_S_GameStart *)pData;

	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_PLAY);

	//�����˿�
	m_cbLeftCardCount = m_cbAllCardCount;
	m_wBankerUser = pGameStart->wBankerUser;
	m_wCurrentUser = m_wBankerUser;
	m_wSiceCount = pGameStart->wSiceCount;
	CopyMemory(m_cbMagicIndex,pGameStart->cbMagicIndex,sizeof(m_cbMagicIndex));
	m_cbActionMask = pGameStart->cbUserAction;
	m_cbFanMagicData = pGameStart->cbFanCardData;

	//���ò���
	if(m_cbMagicMode==1)//�װ��
	{
		m_GameLogic.SetMagicIndex(m_cbMagicIndex);
	}
	else
	{
		m_GameLogic.SetMagicIndex(NULL);
	}


	//�����˿�
	BYTE cbCardCount = (m_pIAndroidUserItem->GetChairID() == m_wBankerUser) ? MAX_COUNT : (MAX_COUNT - 1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData, cbCardCount, m_cbCardIndex);

	if(m_wBankerUser == m_pIAndroidUserItem->GetChairID())
	{
		int nRandTime = 1 + (rand() % __max(1, m_cbTimeOutCard / 2));
#ifdef _DEBUG
		nRandTime = 1;
#endif // _DEBUG
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, 10 + nRandTime) ;
	}

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//Ч����Ϣ
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD) ;

	//��Ϣ����
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	if(pOutCard->bSysOut && pOutCard->wOutCardUser == m_pIAndroidUserItem->GetChairID())
	{
		OnOutCard(pOutCard->cbOutCardData, false);
	}

	//��¼����
	m_cbDiscardCard[pOutCard->wOutCardUser][m_cbDiscardCount[pOutCard->wOutCardUser]] = pOutCard->cbOutCardData;
	m_cbDiscardCount[pOutCard->wOutCardUser]++;

	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_SendCard));
	if (wDataSize != sizeof(CMD_S_SendCard)) return false;

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD) ;

	//��Ϣ����
	CMD_S_SendCard * pSendCard = (CMD_S_SendCard *)pBuffer;

	m_wCurrentUser = pSendCard->wCurrentUser;

	if(pSendCard->wSendCardUser == m_pIAndroidUserItem->GetChairID())
	{		
		m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;	

		m_cbActionMask = pSendCard->cbActionMask;
		m_cbActionCard = pSendCard->cbCardData;
		m_cbEnjoinOutCardCount = 0;
		ZeroMemory(m_cbEnjoinOutCard, sizeof(m_cbEnjoinOutCard));

		if (m_bTing[m_wCurrentUser] && (m_cbActionMask&WIK_CHI_HU) == 0)
		{
			OnOperateCard(WIK_NULL, 0);
		}
		else if (m_bTing[m_wCurrentUser] && (m_cbActionMask&WIK_CHI_HU) != 0)
		{
			AutomatismOutCard();
		}
		else
		{
			int nRandTime = 1 + (rand() % __max(1, m_cbTimeOutCard / 2));
#ifdef _DEBUG
			nRandTime = 1;
#endif // _DEBUG
			m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime) ;
		}
	}

	return true;
}

//������ʾ
bool CAndroidUserItemSink::OnSubOperateNotify(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_OperateNotify));
	if (wDataSize != sizeof(CMD_S_OperateNotify)) return false;

	//��������ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD) ;

	//��������
	CMD_S_OperateNotify * pOperateNotify = (CMD_S_OperateNotify *)pBuffer;

	m_wCurrentUser = INVALID_CHAIR;

	//�û�����
	if (pOperateNotify->cbActionMask != WIK_NULL)
	{
		m_cbActionMask = pOperateNotify->cbActionMask;
		m_cbActionCard = pOperateNotify->cbActionCard;
		
		if(m_bTing[m_pIAndroidUserItem->GetChairID()] && (pOperateNotify->cbActionMask&WIK_CHI_HU) == 0)
		{
			OnOperateCard(WIK_NULL, 0);
		}
		else if (m_bTing[m_pIAndroidUserItem->GetChairID()] && (pOperateNotify->cbActionMask&WIK_CHI_HU) != 0)
		{
			AutomatismOutCard();
		}
		else
		{
			int nRandTime = 1 + (rand() % __max(1, m_cbTimeOperateCard / 2));
#ifdef _DEBUG
			 nRandTime = 1;
#endif // _DEBUG
			m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime);
		}
	}

	return true;
}

//�������
bool CAndroidUserItemSink::OnSubOperateResult(const void * pBuffer, WORD wDataSize)
{
	//Ч����Ϣ
	ASSERT(wDataSize == sizeof(CMD_S_OperateResult));
	if (wDataSize != sizeof(CMD_S_OperateResult)) return false;

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD) ;

	//��Ϣ����
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;

	m_cbActionMask = 0;
	m_cbActionCard = 0;

	BYTE cbOperateCard[3];
	CopyMemory(cbOperateCard, pOperateResult->cbOperateCard, sizeof(cbOperateCard));
	WORD wOperateUser = pOperateResult->wOperateUser;

	if((pOperateResult->cbOperateCode & WIK_GANG) != 0)
	{
		BYTE cbGangKind = WIK_FANG_GANG;

		//���ñ���
		m_wCurrentUser = INVALID_CHAIR;

		//����˿�
		BYTE cbWeaveIndex = 0xFF;
		for (BYTE i = 0; i < m_cbWeaveItemCount[wOperateUser]; i++)
		{
			BYTE cbWeaveKind = m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
			BYTE cbCenterCard = m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard == cbOperateCard[0]) && (cbWeaveKind == WIK_PENG))
			{
				cbWeaveIndex = i;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbParam = WIK_MING_GANG;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind = pOperateResult->cbOperateCode;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCardData[3] = cbCenterCard;
				break;
			}
		}

		//����˿�
		if (cbWeaveIndex == 0xFF)
		{
			//�����ж�
			cbGangKind = (pOperateResult->wProvideUser == wOperateUser) ? WIK_AN_GANG : WIK_FANG_GANG;

			//�����˿�
			cbWeaveIndex = m_cbWeaveItemCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbParam = cbGangKind;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard = cbOperateCard[0];
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind = pOperateResult->cbOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser = pOperateResult->wProvideUser;
			for(BYTE i = 0; i < 4; i++) 
			{
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCardData[i] = cbOperateCard[0];
			}

			if(cbGangKind == WIK_FANG_GANG)
			{
				//��¼����
				m_cbDiscardCount[pOperateResult->wProvideUser]--;
			}
		}

		//�˿�����
		if (m_pIAndroidUserItem->GetChairID() == wOperateUser)
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(cbOperateCard[0])] = 0;
		}
	}
	else if(pOperateResult->cbOperateCode != WIK_NULL)
	{		
		//ɾ���˿�
		if (m_pIAndroidUserItem->GetChairID()==wOperateUser)
		{
			VERIFY(m_GameLogic.RemoveCard(cbOperateCard, CountArray(cbOperateCard), &cbOperateCard[0],1));
			if(!m_GameLogic.RemoveCard(m_cbCardIndex, cbOperateCard, CountArray(cbOperateCard)-1))
			{
				ASSERT(FALSE);
				return false;
			}
		}

		//�������
		BYTE cbWeaveIndex = m_cbWeaveItemCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbParam = WIK_GANERAL;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard = pOperateResult->cbOperateCard[0];
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind = pOperateResult->cbOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser = pOperateResult->wProvideUser;

		m_GameLogic.SortCardList(pOperateResult->cbOperateCard, CountArray(pOperateResult->cbOperateCard));

		CopyMemory(m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCardData, pOperateResult->cbOperateCard, sizeof(pOperateResult->cbOperateCard));
		m_wCurrentUser = wOperateUser;

		//��¼����
		m_cbDiscardCount[pOperateResult->wProvideUser]--;
	}

	if(pOperateResult->cbOperateCode != WIK_NULL)
	{
		if(pOperateResult->cbOperateCode != WIK_GANG && (m_pIAndroidUserItem->GetChairID() == wOperateUser))
		{
			m_cbActionMask = pOperateResult->cbActionMask;
		}
	}

	//����ʱ��
	if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
	{
		int nRandTime = 1 + (rand() % __max(1, m_cbTimeOutCard / 2));
#ifdef _DEBUG
		nRandTime = 1;
#endif // _DEBUG
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime) ;
	}

	return true;
}


//�û�����
bool CAndroidUserItemSink::OnSubListenCard(const void * pBuffer, WORD wDataSize)
{
	//Ч����Ϣ
	ASSERT(wDataSize==sizeof(CMD_S_ListenCard));
	if (wDataSize!=sizeof(CMD_S_ListenCard)) 
		return false;

	//��Ϣ����
	CMD_S_ListenCard * pListenCard = (CMD_S_ListenCard *)pBuffer;

	m_bTing[pListenCard->wListenUser] = pListenCard->bListen;

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameConclude(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameConclude));
	if (wDataSize!=sizeof(CMD_S_GameConclude)) return false;

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);
	
	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_FREE);


	//��������
	CMD_S_GameConclude * pGameConclude=(CMD_S_GameConclude *)pData;

	int nRandTime = 1 + (rand() % __max(1, m_cbTimeStartGame / 2));
#ifdef _DEBUG
	nRandTime = 1;
#endif // _DEBUG
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nRandTime + (pGameConclude->cbMaCount) * 1 + 6);

	return true;
}

//�û��й�
bool CAndroidUserItemSink::OnSubTrustee(const void * pBuffer,WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_Trustee));
	if (wDataSize!=sizeof(CMD_S_Trustee)) return false;

	if(m_pIAndroidUserItem->GetGameStatus() != GAME_SCENE_PLAY)
		return true;

	//��Ϣ����
	CMD_S_Trustee * pTrustee=(CMD_S_Trustee *)pBuffer;

	return true;
}


bool CAndroidUserItemSink::OnSubTingData(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_TING_DATA));
	if(wDataSize!=sizeof(CMD_S_TING_DATA)) return false;


	return true;
}

bool CAndroidUserItemSink::OnSubHuData(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_HU_DATA));
	if(wDataSize!=sizeof(CMD_S_HU_DATA)) return false;

	CMD_S_HU_DATA *pPack  = (CMD_S_HU_DATA*)pBuffer;

	return true;
}

//����
void CAndroidUserItemSink::OnOutCard( BYTE cbOutCard, bool bSend )
{
	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//���ñ���
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//ɾ����
	m_GameLogic.RemoveCard(m_cbCardIndex, cbOutCard);

	//��������
	if(bSend)
	{		
		CMD_C_OutCard OutCard;
		OutCard.cbCardData=cbOutCard;
		if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)) )
		{
			ASSERT( FALSE );
			return ;
		}
	}
	
	return ;
}

//������
void CAndroidUserItemSink::OnOperateCard( BYTE cbOperateCode,BYTE cbOperateCard )
{
	//ɾ��ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//���ñ���
	m_cbActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//��������
	BYTE cbCard[3] = {cbOperateCard,0,0};

	//����ǳ���
	if( cbOperateCode & WIK_LEFT )
	{
		cbCard[1] = cbOperateCard+1;
		cbCard[2] = cbOperateCard+2;
	}
	else if( cbOperateCode & WIK_CENTER )
	{
		cbCard[1] = cbOperateCard-1;
		cbCard[2] = cbOperateCard+1;
	}
	else if( cbOperateCode & WIK_RIGHT )
	{
		cbCard[1] = cbOperateCard-2;
		cbCard[2] = cbOperateCard-1;
	}

	//��������
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	CopyMemory( OperateCard.cbOperateCard,cbCard,sizeof(cbCard) );
	if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard)) )
	{
		ASSERT( FALSE );
		return ;
	}

	return ;
}

//��������
bool CAndroidUserItemSink::SearchTingCard( tagTingCardResult &TingCardResult )
{
	//��������
	ZeroMemory(&TingCardResult,sizeof(TingCardResult));
	BYTE cbAbandonCardCount=0;

	//�����˿�
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,m_cbCardIndex,sizeof(cbCardIndexTemp));

	BYTE cbCardCount = m_GameLogic.GetCardCount(cbCardIndexTemp);
	if( (cbCardCount+1)%3 == 0 )
	{
		//���Ʒ���
		for (BYTE i=0;i<MAX_INDEX-MAX_HUA_INDEX;i++)
		{
			//���ƹ���
			if (cbCardIndexTemp[i]==0) continue;

			//���ƴ���
			cbCardIndexTemp[i]--;

			//�����ж�
			bool bHuCard = false;
			cbAbandonCardCount = TingCardResult.cbAbandonCount;
			CChiHuRight chr;
			for (BYTE j=0;j<MAX_INDEX-MAX_HUA_INDEX;j++)
			{
				//���Ʒ���
				BYTE cbCurrentCard=m_GameLogic.SwitchToCardData(j);
				BYTE cbHuCardKind=m_GameLogic.AnalyseChiHuCard(cbCardIndexTemp,m_WeaveItemArray[wMeChairId],m_cbWeaveItemCount[wMeChairId],cbCurrentCard,chr);

				//����ж�
				if (cbHuCardKind!=WIK_NULL)
				{
					bHuCard = true;
					TingCardResult.cbTingCard[cbAbandonCardCount][TingCardResult.cbTingCardCount[cbAbandonCardCount]++] = cbCurrentCard;
				}
			}
			if( bHuCard == true )
			{
				TingCardResult.cbAbandonCard[TingCardResult.cbAbandonCount++] = m_GameLogic.SwitchToCardData(i);
			}
			//��ԭ����
			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		//�����ж�
		cbAbandonCardCount = TingCardResult.cbAbandonCount;
		CChiHuRight chr;
		for (BYTE j=0;j<MAX_INDEX-MAX_HUA_INDEX;j++)
		{
			//���Ʒ���
			BYTE cbCurrentCard=m_GameLogic.SwitchToCardData(j);
			BYTE cbHuCardKind=m_GameLogic.AnalyseChiHuCard(cbCardIndexTemp,m_WeaveItemArray[wMeChairId],m_cbWeaveItemCount[wMeChairId],cbCurrentCard,chr);

			//����ж�
			if (cbHuCardKind!=WIK_NULL)
			{
				TingCardResult.cbTingCard[cbAbandonCardCount][TingCardResult.cbTingCardCount[cbAbandonCardCount]++] = cbCurrentCard;
			}
		}
	}

	//����ʣ����
	BYTE n = 0;
	while( TingCardResult.cbTingCardCount[n] > 0 )
	{
		TingCardResult.cbRemainCount[n] = TingCardResult.cbTingCardCount[n]*4;

		for( BYTE i = 0; i < TingCardResult.cbTingCardCount[n]; i++ )
		{
			BYTE cbCardData = TingCardResult.cbTingCard[n][i];
			int nCardDataCount = 4;

			//���Լ���
			if( cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(cbCardData)] > 0 )
			{
				TingCardResult.cbRemainCount[n] -= cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(cbCardData)];
				nCardDataCount -= cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(cbCardData)];
				ASSERT( nCardDataCount >= 0 );
			}

			for( BYTE j = 0; j < GAME_PLAYER; j++ )
			{
				//�������
				for( BYTE k = 0; k < m_cbWeaveItemCount[j]; k++ )
				{
					if( m_WeaveItemArray[j][k].cbWeaveKind==WIK_GANG)
					{
						if( m_WeaveItemArray[j][k].cbCenterCard == cbCardData )
						{
							TingCardResult.cbRemainCount[n] -= 4;
							nCardDataCount -= 4;
							ASSERT( nCardDataCount >= 0 );
						}
					}					
					else if( m_WeaveItemArray[j][k].cbWeaveKind==WIK_PENG)
					{
						if( m_WeaveItemArray[j][k].cbCenterCard == cbCardData )
						{
							TingCardResult.cbRemainCount[n] -= 3;
							nCardDataCount -= 3;
							ASSERT( nCardDataCount >= 0 );
						}
					}
					else if( m_WeaveItemArray[j][k].cbWeaveKind==WIK_LEFT)
					{
						if( m_WeaveItemArray[j][k].cbCenterCard == cbCardData || m_WeaveItemArray[j][k].cbCenterCard + 1 == cbCardData || m_WeaveItemArray[j][k].cbCenterCard + 2 == cbCardData)
						{
							TingCardResult.cbRemainCount[n] -= 1;
							nCardDataCount -= 1;
							ASSERT( nCardDataCount >= 0 );
						}
					}
					else if( m_WeaveItemArray[j][k].cbWeaveKind==WIK_CENTER)
					{
						if( m_WeaveItemArray[j][k].cbCenterCard - 1 == cbCardData || m_WeaveItemArray[j][k].cbCenterCard == cbCardData || m_WeaveItemArray[j][k].cbCenterCard + 1 == cbCardData)
						{
							TingCardResult.cbRemainCount[n] -= 1;
							nCardDataCount -= 1;
							ASSERT( nCardDataCount >= 0 );
						}
					}
					else if( m_WeaveItemArray[j][k].cbWeaveKind==WIK_RIGHT)
					{
						if( m_WeaveItemArray[j][k].cbCenterCard - 2 == cbCardData || m_WeaveItemArray[j][k].cbCenterCard - 1 == cbCardData || m_WeaveItemArray[j][k].cbCenterCard == cbCardData)
						{
							TingCardResult.cbRemainCount[n] -= 1;
							nCardDataCount -= 1;
							ASSERT( nCardDataCount >= 0 );
						}
					}
					else
					{
						ASSERT(NULL);
					}
				}

				//��������
				for( int k = 0; k < m_cbDiscardCount[j]; k++ )
				{
					if( cbCardData == m_cbDiscardCard[j][k] )
					{
						TingCardResult.cbRemainCount[n]--;
						nCardDataCount--;
						ASSERT( nCardDataCount >= 0 );
					}
				}
			}
		}

		n++;
	}

	return true;
}

//��������
bool CAndroidUserItemSink::SearchOutCard( tagOutCardResult &OutCardResult )
{
	//��ʼ��
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	ASSERT( wMeChairId != INVALID_CHAIR );
	if( wMeChairId == INVALID_CHAIR ) return false;

	//�жϺ�
	if( (m_cbActionMask&WIK_CHI_HU) != WIK_NULL )
	{
		OutCardResult.cbOperateCode = WIK_CHI_HU;
		OutCardResult.cbOperateCard = m_cbActionCard;
		return true;
	}

	//ת������
	BYTE byCard[MAX_COUNT],byCardCount = 0;
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		for( BYTE j = 0; j < m_cbCardIndex[i]; j++ )
		{
			byCard[byCardCount++] = i;
		}
	}

	//�ж���
	if( WIK_LISTEN == m_GameLogic.AnalyseTingCard(m_cbCardIndex,m_WeaveItemArray[wMeChairId],m_cbWeaveItemCount[wMeChairId]) )
	{
		tagTingCardResult TingCardResult;
		if( SearchTingCard(TingCardResult) )
		{
			BYTE cbMostIndex = 0;
			int nMostCount = -1;
			BYTE i = 0;
			while(true)
			{
				if( TingCardResult.cbTingCardCount[i] == 0 ) break;

				if( TingCardResult.cbRemainCount[i] > nMostCount )
				{
					cbMostIndex = i;
					nMostCount = TingCardResult.cbRemainCount[i];
				}

				i++;
			}
			//���ƿ���
			if( nMostCount > 0 )
			{
				//��������
				if( wMeChairId != m_wCurrentUser ) return false;

				OutCardResult.cbOperateCode = WIK_NULL;
				OutCardResult.cbOperateCard = TingCardResult.cbAbandonCard[cbMostIndex];
				return true;
			}
			//������
			else if( wMeChairId == m_wCurrentUser )
			{
				//����AI
				CAndroidAI AndroidAi;
				AndroidAi.SetEnjoinOutCard(m_cbEnjoinOutCard, m_cbEnjoinOutCardCount);
				AndroidAi.SetCardData(byCard,byCardCount);
				AndroidAi.Think();
				//������в���
				BYTE byBadlyCard = 0xff;
				if( byCardCount <= 2 )
					byBadlyCard = AndroidAi.GetBadlyCard();
				else
					byBadlyCard = AndroidAi.GetBadlyIn2Card();
				if( 0xff != byBadlyCard )
				{
					OutCardResult.cbOperateCode = WIK_NULL;
					OutCardResult.cbOperateCard = m_GameLogic.SwitchToCardData(byBadlyCard);
					return true;
				}
				else
				{
					//�������ֻ����в���
					byBadlyCard = AndroidAi.GetBadlyIn3Card();
					if( 0xff != byBadlyCard )
					{
						OutCardResult.cbOperateCode = WIK_NULL;
						OutCardResult.cbOperateCard = m_GameLogic.SwitchToCardData(byBadlyCard);
						return true;
					}
				}
			}
		}
	}

	BYTE cbActionCard = m_cbActionCard;

	//������ֲ����÷�
	BYTE cbOperateCode[] = { WIK_GANG,WIK_PENG,WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_NULL };
	int nOperateScore[] = { 0,0,0,0,0,0 };
	//����ԭʼ��
	CAndroidAI AndroidAi;
	AndroidAi.SetEnjoinOutCard(m_cbEnjoinOutCard, m_cbEnjoinOutCardCount);
	AndroidAi.SetCardData(byCard,byCardCount);
	AndroidAi.Think();
	int nOrgScore = AndroidAi.GetMaxScore();
	//�жϸ�
	if( m_cbActionMask & WIK_GANG )
	{
		if( m_wCurrentUser == wMeChairId )
		{
			tagGangCardResult GangCardResult;
			m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairId],m_cbWeaveItemCount[wMeChairId],m_cbActionCard, GangCardResult);

			//Ѱ����߷ָ���
			BYTE cbGangCard = 0;
			int nMaxGangScore = -1;
			for( BYTE i = 0; i < GangCardResult.cbCardCount; i++ )
			{
				ASSERT( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i])] > 0 );
				if( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i])] == 0 ) throw 0;

				//����ܺ�÷�
				AndroidAi.SetCardData(byCard,byCardCount);
				AndroidAi.SetAction(WIK_GANG,m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i]));
				AndroidAi.Think();

				int nScore = AndroidAi.GetMaxScore();
				if( nScore > nMaxGangScore )
				{
					nMaxGangScore = nScore;
					cbGangCard = GangCardResult.cbCardData[i];
				}
			}

			ASSERT(nMaxGangScore!=-1&&cbGangCard!=0);
			cbActionCard = cbGangCard;
			nOperateScore[0] = nMaxGangScore-nOrgScore;
		}
		else
		{
			ASSERT( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)] > 0 );
			if( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)] == 0 ) throw 0;

			//����ܺ�÷�
			AndroidAi.SetCardData(byCard,byCardCount);
			AndroidAi.SetAction(WIK_GANG,m_GameLogic.SwitchToCardIndex(cbActionCard));
			AndroidAi.Think();
			nOperateScore[0] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//�ж���
	if( m_cbActionMask & WIK_PENG )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_PENG,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[1] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//���
	if( m_cbActionMask & WIK_LEFT )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_LEFT,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[2] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//�г�
	if( m_cbActionMask & WIK_CENTER )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_CENTER,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[3] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//�ҳ�
	if( m_cbActionMask & WIK_RIGHT )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_RIGHT,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[4] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}

	//��������
	BYTE cbOutCardData = cbActionCard;
	if( m_wCurrentUser == wMeChairId )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
			{
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
				ASSERT( byBadlyIndex != 0xff );
				if( byBadlyIndex == 0xff ) throw 0;
			}
		}
		AndroidAi.RemoveCardData( byBadlyIndex );
		AndroidAi.Think();
		nOperateScore[5] = AndroidAi.GetMaxScore()-nOrgScore;
		cbOutCardData = m_GameLogic.SwitchToCardData(byBadlyIndex);
	}

	//��ȡ��߷ֲ���
	BYTE cbIndex = 0;
	for( BYTE i = 1; i < CountArray(nOperateScore); i++ )
	{
		if( nOperateScore[cbIndex] < nOperateScore[i] )
		{
			cbIndex = i;
		}
	}
	if( (cbOperateCode[cbIndex]&m_cbActionMask) && ( cbOperateCode[cbIndex]==WIK_GANG || nOperateScore[cbIndex] > 0 ) )
	{
		OutCardResult.cbOperateCode = cbOperateCode[cbIndex];
		OutCardResult.cbOperateCard = cbActionCard;
		return true;
	}
	else
	{
		if( m_wCurrentUser == wMeChairId )
		{
			OutCardResult.cbOperateCard = cbOutCardData;
			return true;
		}
		else
		{
			return false;
		}
	}

	ASSERT( FALSE );
	throw 0;
}

//�Զ�����
bool CAndroidUserItemSink::AutomatismOutCard()
{
	tagOutCardResult OutCardResult;
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
#ifdef	_DEBUG
	BYTE byCardCount = m_GameLogic.GetCardCount( m_cbCardIndex );
	if( wMeChairId == m_wCurrentUser )
		ASSERT( (byCardCount+1)%3 == 0 );
	else ASSERT( byCardCount%3 == 1 );
#endif

	try
	{
		//��������
		if( SearchOutCard(OutCardResult) )
		{
			if( OutCardResult.cbOperateCode != WIK_NULL )
			{
				//Ч��
				ASSERT( OutCardResult.cbOperateCode&m_cbActionMask );
				if( !(OutCardResult.cbOperateCode&m_cbActionMask) ) throw 0;

				//��Ӧ����
				OnOperateCard(OutCardResult.cbOperateCode,OutCardResult.cbOperateCard);
			}
			else
			{
				//Ч��
				ASSERT( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(OutCardResult.cbOperateCard)] > 0 );
				if( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(OutCardResult.cbOperateCard)] == 0 ) throw 0;

				//����
				OnOutCard(OutCardResult.cbOperateCard, true);
			}
		}
		else
		{
			//Ч��
			ASSERT( wMeChairId != m_wCurrentUser );
			if( wMeChairId == m_wCurrentUser ) throw 0;

			//��Ӧ����
			OnOperateCard(WIK_NULL,0);
		}
	}
	catch(...)
	{
		ASSERT(FALSE);
		//�쳣����
		if( wMeChairId == m_wCurrentUser )
		{
			for( BYTE i = 0; i < MAX_INDEX-MAX_HUA_INDEX; i++ )
			{
				if( m_cbCardIndex[i] > 0 )
				{
					OnOutCard( m_GameLogic.SwitchToCardData(i), true );
					return true;
				}
			}
		}
		else
		{
			OnOperateCard( WIK_NULL,0 );
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
