#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
//#define IDI_FREE					1									//����ʱ��
#define IDI_PLACE_JETTON			102									//��עʱ��
//#define IDI_DISPATCH_CARD			301									//����ʱ��

#define IDI_PLACE_JETTON1			103									//��ע��ʱ
#define IDI_PLACE_JETTON2			104									//��ע��ʱ
#define IDI_PLACE_JETTON3			105									//��ע��ʱ
#define IDI_PLACE_JETTON4			106									//��ע��ʱ
#define IDI_PLACE_JETTON5			107									//��ע��ʱ

#define IDI_GIVEUP_BANKER            101									//��ׯ

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink():
m_wCurrentBanker(INVALID_CHAIR)
{


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
	m_wCurrentBanker = INVALID_CHAIR;
	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	
	switch(nTimerID)
	{
	case IDI_GIVEUP_BANKER:
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);
			m_pIAndroidUserItem->SendSocketData(SUB_C_CANCEL_BANKER);
		}
	    break;
	default:
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);

			int JetScore[]={100, 100, 100, 1000, 100, 1000, 100, 1000, 100, 100};
			int JetScoreMax[] = { 1000, 10000, 100000, 200000, 1000, 10000, 100000, 200000, 500000, 500000 };
			//������Ϣ
			CMD_C_PlaceJetton JettonInfo;
			INT32 nRandGameType = RandInt(0,1);
			if (nRandGameType == eGambleType_AnimalGame)
			{
				JettonInfo.eGamble = eGambleType_AnimalGame;
				JettonInfo.eEnjoyGameInfo = eEnjoyGameType_Invalid;

				JettonInfo.eGamble = eGambleType_AnimalGame;
				JettonInfo.stAnimalInfo.eAnimal = (eAnimalType) RandInt(eAnimalType_Lion,eAnimalType_Rabbit);
				JettonInfo.stAnimalInfo.eColor = (eColorType)RandInt(eColorType_Red,eColorType_Yellow);
				JettonInfo.eEnjoyGameInfo = eEnjoyGameType_Invalid;
				if (m_lMinEnterScore >200000)
				{
					JettonInfo.iPlaceJettonScore = JetScoreMax[RandInt(0, 9)];
				}
				else
				{
					JettonInfo.iPlaceJettonScore = JetScore[RandInt(0, 9)];
				}
				
			}
			else
			{
				JettonInfo.eGamble = eGambleType_EnjoyGame;
				JettonInfo.stAnimalInfo.eAnimal = (eAnimalType)0;
				JettonInfo.stAnimalInfo.eColor = (eColorType)0;
				JettonInfo.eEnjoyGameInfo = (eEnjoyGameType)RandInt(eEnjoyGameType_Zhuang,eEnjoyGameType_He);
				if (m_lMinEnterScore >200000)
				{
					JettonInfo.iPlaceJettonScore = JetScoreMax[RandInt(0, 9)];
				}
				else
				{
					JettonInfo.iPlaceJettonScore = JetScore[RandInt(0, 9)];
				}
			}

			m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_JETTON, &JettonInfo, sizeof(JettonInfo));

		}
		break;
	}
	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_FREE:			//��Ϸ���� 
		{
			return OnSubGameFree(pBuffer, wDataSize);
		}
	case SUB_S_GAME_START:			//��Ϸ��ʼ
		{
			return OnSubGameStart(pBuffer, wDataSize);
		}
	case SUB_S_PLACE_JETTON:		//�û���ע
		{
			return OnSubPlaceJetton(pBuffer, wDataSize);
		}
	case SUB_S_APPLY_BANKER:		//������ׯ 
		{
			return OnSubUserApplyBanker(pBuffer,wDataSize);
		}
	case SUB_S_CANCEL_BANKER:		//ȡ����ׯ 
		{
			return OnSubUserCancelBanker(pBuffer,wDataSize);
		}
	case SUB_S_CHANGE_BANKER:		//�л�ׯ�� 
		{
			return OnSubChangeBanker(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:			//��Ϸ���� 
		{
			return OnSubGameEnd(pBuffer, wDataSize);
		}
	case SUB_S_SEND_RECORD:			//��Ϸ��¼ (����)
		{
			return true;
		}
	case SUB_S_PLACE_JETTON_FAIL:	//��עʧ�� (����)
		{
			return true;
		}
	case SUB_S_GAME_OPTION:
		{
			return OnSubGameOption(pBuffer, wDataSize);
		}
	}

	//�������
	ASSERT(FALSE);

	return true;

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
	case GAME_STATUS_FREE:			//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��Ϣ����
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//m_lUserLimitScore = pStatusFree->lUserMaxScore;
			//m_lAreaLimitScore = pStatusFree->lAreaLimitScore;
			//m_lBankerCondition = pStatusFree->lApplyBankerCondition;

			//memcpy(m_szRoomName, pStatusFree->szGameRoomName, sizeof(m_szRoomName));

			//ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, true);

			////MyDebug(_T("��������ׯ(Free) %d [%d %d] [%d %d]"), m_pIAndroidUserItem->GetChairID(), m_nWaitBanker, 
			////	m_nRobotWaitBanker, m_stlApplyBanker, m_nRobotApplyBanker);

			////��ׯ����
			//if (pStatusFree->wBankerUser == INVALID_CHAIR)
			//{
			//	if (m_bRobotBanker && m_nRobotWaitBanker == 0  && m_stlApplyBanker < m_nRobotApplyBanker)
			//	{
			//		//�Ϸ��ж�
			//		IServerUserItem *pIUserItemBanker = m_pIAndroidUserItem->GetMeUserItem();
			//		if (pIUserItemBanker->GetUserScore()->lScore > m_lBankerCondition) 
			//		{
			//			//��������ׯ
			//			m_nBankerCount = 0;
			//			m_stlApplyBanker++;

			//			BYTE cbTime = (pStatusFree->cbTimeLeave>0?(rand()%pStatusFree->cbTimeLeave+1):2);
			//			if (cbTime == 0) cbTime = 2;

			//			m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST_BANKER, cbTime);
			//		}
			//	}
			//}

			return true;
		}
	case GAME_STATUS_PLAY:		//��Ϸ״̬
	case GS_GAME_END:		//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			////��Ϣ����
			//CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			////ׯ����Ϣ
			//m_wCurrentBanker = pStatusPlay->wBankerUser;

			//m_lUserLimitScore = pStatusPlay->lUserMaxScore;
			//m_lAreaLimitScore = pStatusPlay->lAreaLimitScore;
			//m_lBankerCondition = pStatusPlay->lApplyBankerCondition;

			//memcpy(m_szRoomName, pStatusPlay->szGameRoomName, sizeof(m_szRoomName));

			//ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, true);

			return true;
		}
	}

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

// ������Ҷ���ʼ��
void CAndroidUserItemSink::OnEventAllUserStart(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return ;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//��ȡ����
	//if (m_bRefreshCfg)
	//	ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, false);
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameFree));
	if (wDataSize!=sizeof(CMD_S_GameFree)) return false;

	//��Ϣ����
	CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;

	//�������������ׯ����һ��������ׯ
	if(pGameFree->cbCanCancelBank == 1)
	{
		int nRandGiveBanker = RandInt(1,100);
		if (nRandGiveBanker < 30)
		{
			m_pIAndroidUserItem->SetGameTimer(IDI_GIVEUP_BANKER, 2);
		}
	}
	return true;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{	
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��Ϣ����
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	srand(GetTickCount());

	if (pGameStart->cbBankerFlag == 1)
	{
		m_wCurrentBanker = m_pIAndroidUserItem->GetChairID();
	}
	else
	{
		//������ע����
		int nElapse = 0;
		WORD wMyID = m_pIAndroidUserItem->GetChairID();
		BYTE m_nChipTime = (rand() + wMyID) % 5 + 3;

		//����ʱ��
		int nTimeGrid = int(pGameStart->cbTimeLeave - 2) * 800 / m_nChipTime;						//ʱ���,ǰ2�벻��ע,����-2,800��ʾ��������עʱ�䷶Χǧ�ֱ�
		for (int i = 0; i < m_nChipTime; i++)
		{
			int nRandRage = int(nTimeGrid * i / (1500 * sqrt((double)m_nChipTime))) + 1;		//������Χ
			nElapse = 2 + (nTimeGrid*i) / 1000 + ((rand() + wMyID) % (nRandRage * 2) - (nRandRage - 1));
			ASSERT(nElapse >= 2 && nElapse <= pGameStart->cbTimeLeave);
			if (nElapse < 2 || nElapse > pGameStart->cbTimeLeave)	continue;

			m_pIAndroidUserItem->SetGameTimer(IDI_PLACE_JETTON + i + 1, nElapse);
		}

		m_wCurrentBanker = INVALID_CHAIR;
	}


	////�Լ���ׯ������ע������
	//if (pGameStart->wBankerUser == m_pIAndroidUserItem->GetChairID() || pGameStart->nChipRobotCount <= 0)
	//	return true;

	////���ñ���
	//m_lMaxChipBanker = pGameStart->lBankerScore/m_RobotInfo.nMaxTime;
	//m_lMaxChipUser = __min(pGameStart->lUserMaxScore/m_RobotInfo.nMaxTime, m_lUserLimitScore);
	//m_wCurrentBanker = pGameStart->wBankerUser;
	//m_nChipTimeCount = 0;
	//ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
	//for (int i = 0; i < AREA_COUNT; i++)
	//	m_lAreaChip[i] = m_lAreaLimitScore;

	////ϵͳ��ׯ
	//if (pGameStart->wBankerUser == INVALID_CHAIR)
	//{
	//	m_stlApplyBanker = 0;
	//	m_lMaxChipBanker = 2147483647/m_RobotInfo.nMaxTime;
	//}
	//else
	//	m_lMaxChipUser = __min( m_lMaxChipUser, m_lMaxChipBanker );

	////������ע����
	//int nElapse = 0;												
	//WORD wMyID = m_pIAndroidUserItem->GetChairID();

	//if (m_nRobotBetTimeLimit[0] == m_nRobotBetTimeLimit[1])
	//	m_nChipTime = m_nRobotBetTimeLimit[0];
	//else
	//	m_nChipTime = (rand()+wMyID)%(m_nRobotBetTimeLimit[1]-m_nRobotBetTimeLimit[0]+1) + m_nRobotBetTimeLimit[0];
	//ASSERT(m_nChipTime>=0);		
	//if (m_nChipTime <= 0)	return false;								//��ȷ,2����������
	//if (m_nChipTime > MAX_CHIP_TIME)	m_nChipTime = MAX_CHIP_TIME;	//�޶�MAX_CHIP����ע

	////���㷶Χ
	//if (!CalcJettonRange(__min(m_lMaxChipBanker, m_lMaxChipUser), m_lRobotJettonLimit, m_nChipTime, m_nChipLimit))
	//	return true;

	////����ʱ��
	//int nTimeGrid = int(pGameStart->cbTimeLeave-2)*800/m_nChipTime;		//ʱ���,ǰ2�벻��ע,����-2,800��ʾ��������עʱ�䷶Χǧ�ֱ�
	//for (int i = 0; i < m_nChipTime; i++)
	//{
	//	int nRandRage = int( nTimeGrid * i / (1500*sqrt((double)m_nChipTime)) ) + 1;		//������Χ
	//	nElapse = 2 + (nTimeGrid*i)/1000 + ( (rand()+wMyID)%(nRandRage*2) - (nRandRage-1) );
	//	ASSERT(nElapse>=2&&nElapse<=pGameStart->cbTimeLeave);
	//	if (nElapse < 2 || nElapse > pGameStart->cbTimeLeave)	continue;

	//	m_pIAndroidUserItem->SetGameTimer(IDI_PLACE_JETTON+i+1, nElapse);
	//}

	//ASSERT( MyDebug(_T("������ %d ��ע���� %d ��Χ [%d %d] ������ %d ���� [%I64d %I64d] ��ׯ [%d %d]"), wMyID, m_nChipTime, m_nChipLimit[0], m_nChipLimit[1], 
	//	pGameStart->nChipRobotCount, m_lMaxChipBanker, m_lMaxChipUser, m_stlApplyBanker, m_nRobotApplyBanker) );

	return true;
}

//�û���ע
bool CAndroidUserItemSink::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJetton));
	if (wDataSize!=sizeof(CMD_S_PlaceJetton)) return false;

	//��Ϣ����
	CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;

	////���ñ���
	//m_lMaxChipBanker -= pPlaceJetton->lJettonScore;
	//m_lAreaChip[pPlaceJetton->cbJettonArea-1] -= pPlaceJetton->lJettonScore;
	//if (pPlaceJetton->wChairID == m_pIAndroidUserItem->GetChairID())
	//	m_lMaxChipUser -= pPlaceJetton->lJettonScore;

	return true;
}

//��עʧ��
bool CAndroidUserItemSink::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//��Ϣ����
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//if ( m_wCurrentBanker == INVALID_CHAIR )
	//{
	//	//��ׯ
	//	m_nWaitBanker++;

	//	//MyDebug(_T("��������ׯ(End) %d [%d %d] [%d %d]"), m_pIAndroidUserItem->GetChairID(), m_nWaitBanker, 
	//	//	m_nRobotWaitBanker, m_stlApplyBanker, m_nRobotApplyBanker);

	//	//��������ׯ
	//	if ( m_bRobotBanker && m_nWaitBanker >= m_nRobotWaitBanker && m_stlApplyBanker < m_nRobotApplyBanker)
	//	{
	//		//�Ϸ��ж�
	//		IServerUserItem *pIUserItemBanker = m_pIAndroidUserItem->GetMeUserItem();
	//		if (pIUserItemBanker->GetUserScore()->lScore > m_lBankerCondition) 
	//		{
	//			//��������ׯ
	//			m_nBankerCount = 0;
	//			m_stlApplyBanker++;
	//			m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST_BANKER, (rand() % pGameEnd->cbTimeLeave) + 1);
	//		}
	//	}
	//}
	//else if ( m_wCurrentBanker == m_pIAndroidUserItem->GetChairID() )
	//{
	//	m_nBankerCount++;
	//	if ( m_nBankerCount >= m_nRobotBankerCount )
	//	{
	//		//��������ׯ
	//		m_nBankerCount = 0;
	//		m_pIAndroidUserItem->SetGameTimer(IDI_GIVEUP_BANKER, pGameEnd->cbTimeLeave + 1);
	//	}
	//}
	//else if ( m_wCurrentBanker != INVALID_CHAIR )
	//{
	//	//��������ׯ
	//	m_nWaitBanker = 0;
	//}

	return true;
}

//������ׯ
bool CAndroidUserItemSink::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) return false;

	//��Ϣ����
	CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;

	////�Լ��ж�
	//if (m_pIAndroidUserItem->GetChairID()==pApplyBanker->wApplyUser) 
	//	m_bMeApplyBanker=true;

	return true;
}

//ȡ����ׯ
bool CAndroidUserItemSink::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
	if (wDataSize!=sizeof(CMD_S_CancelBanker)) return false;

	//��Ϣ����
	CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;

	////�Լ��ж�
	//if (lstrcmp(m_pIAndroidUserItem->GetMeUserItem()->GetAccounts(),pCancelBanker->szCancelUser)==0) 
	//	m_bMeApplyBanker=false;

	return true;
}

//�л�ׯ��
bool CAndroidUserItemSink::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
	if (wDataSize!=sizeof(CMD_S_ChangeBanker)) return false;

	//��Ϣ����
	CMD_S_ChangeBanker * pChangeBanker = (CMD_S_ChangeBanker *)pBuffer;

	//if ( m_wCurrentBanker == m_pIAndroidUserItem->GetChairID() && m_wCurrentBanker != pChangeBanker->wBankerUser )
	//{
	//	m_stlApplyBanker--;
	//}
	//m_wCurrentBanker = pChangeBanker->wBankerUser;
	//m_nWaitBanker = 0;

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameOption(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_Game_Option));
	if (wDataSize != sizeof(CMD_S_Game_Option)) return false;

	//��Ϣ����
	CMD_S_Game_Option* GameOption = (CMD_S_Game_Option *)pBuffer;

	if (GameOption != NULL)
	{
		m_lMinTableScore = GameOption->lMinTableScore;
		m_lMinEnterScore = GameOption->lMinEnterScore;
		m_lMaxEnterScore = GameOption->lMaxEnterScore;
	}

	return true;
}

bool CAndroidUserItemSink::IsBanker(IAndroidUserItem * pIAndroidUserItem)
{
	if (pIAndroidUserItem->GetChairID() == m_wCurrentBanker)
	{
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
