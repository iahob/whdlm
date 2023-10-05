#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <locale>
#include <objbase.h>
#include <math.h>


//////////////////////////////////////////////////////////////////////////

//��ʱ��
#define IDI_CHECK_BAG			1			//�������
#define IDI_GAME_END			2			//��Ϸ����
#define IPC_PACKAGE				4096
//////////////////////////////////////////////////////////////////////////

int					CTableFrameSink::m_nUpdateStorageCount = 1;													//���������
LONGLONG			CTableFrameSink::m_lInitSystemStorage = 0;													//��ʼϵͳ���
LONGLONG			CTableFrameSink::m_lLastInitSystemStorage;													//��ʼϵͳ���
LONGLONG			CTableFrameSink::m_lSystemStorage = 0;														//ϵͳ���
LONGLONG			CTableFrameSink::m_lLastInitUserStorage = 0;												//��ʼ��ҿ��
LONGLONG			CTableFrameSink::m_lInitUserStorage = 0;													//��ʼ��ҿ��
LONGLONG			CTableFrameSink::m_lUserStorage = 0;														//��ҿ��
int					CTableFrameSink::m_nInitParameterK = 20;													//��ʼ����K(�ٷֱ�)
int					CTableFrameSink::m_nParameterK = 20;														//����K(�ٷֱ�)
LONGLONG			CTableFrameSink::m_lSystemStorageEndScore = 0;												//����ֹ
bool				CTableFrameSink::m_bSystemStorageEnd = false;												//����ֹ
CTime				CTableFrameSink::m_tStartDebugTime;															//����������ʱ��
CTime				CTableFrameSink::m_tResetSystemStorage;														//�������ʱ��
CTime				CTableFrameSink::m_tUpdateSystemStorage;													//������ʱ��
LONGLONG			CTableFrameSink::m_lStorageCurrent = 0;														//��ǰ��Ӯ

CMap<DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo> CTableFrameSink::m_mapRoomUserInfo;						//���USERIDӳ�������Ϣ
CList<tagRoomDebugInfo, tagRoomDebugInfo> CTableFrameSink::m_listRoomDebugInfo;									//�������ӳ��
CList<tagHistoryRoomUserInfo, tagHistoryRoomUserInfo&> CTableFrameSink::m_listHistoryRoomUserInfo;				//��ҵ��Լ�¼
CList<tagHistoryRoomDebugInfo, tagHistoryRoomDebugInfo&> CTableFrameSink::m_listHistoryRoomDebugInfo;			//������Լ�¼
CList<tagHistoryRoomDebugInfo, tagHistoryRoomDebugInfo&> CTableFrameSink::m_listHistorySysDebugInfo;			//ϵͳ���Լ�¼
DWORD				CTableFrameSink::m_dwUserDebugIndex = 1;													//��ҵ�������
DWORD				CTableFrameSink::m_dwRoomDebugIndex = 1;													//�����������
DWORD				CTableFrameSink::m_dwSysDebugIndex = 1;														//ϵͳ��������

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
	m_dwBeginTime = 0;
	m_wBankerUser = INVALID_CHAIR;
	m_lBagScore = 0;
	m_cbThunderPoint = INVALID_BYTE;
	m_cbAllGetCount = 0;
	memset(m_wGetUser, INVALID_CHAIR, sizeof(m_wGetUser));
	ZeroMemory(m_dUserScore, sizeof(m_dUserScore));
	ZeroMemory(m_dBagDetail, sizeof(m_dBagDetail));
	ZeroMemory(m_bThunderPoint, sizeof(m_bThunderPoint));
	ZeroMemory(m_lBagTotalScore, sizeof(m_lBagTotalScore));

	//�����������
	srand(time(NULL));	
}

//��������
CTableFrameSink::~CTableFrameSink()
{
	
}

//�ͷŶ���
VOID CTableFrameSink::Release()
{
}

//�ӿڲ�ѯ
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);

	//�����ж�
	if (m_pITableFrame == NULL)
	{
		CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"), TraceLevel_Exception);
		return false;
	}

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

	ASSERT (m_pGameServiceOption != NULL);

	m_pCustomRule = (tagCustomRule*)(m_pITableFrame->GetCustomRule());
	ASSERT(m_pCustomRule != NULL);

	CString cc;
	cc.Format(_T("��������:����ʱ��%d��,�������%d,������%I64d~%I64d,�⸶����%lf,�һ�����%d,�׺�λ��:%s"), m_pCustomRule->cbPlayTime, m_pCustomRule->cbBagCount,
		m_pCustomRule->lScoreRangeMin, m_pCustomRule->lScoreRangeMax, m_pCustomRule->dBagTimes, m_pCustomRule->nScoreRatio,
		m_pCustomRule->cbThunderPosition == 0 ? _T("С�����һλ") : _T("С����ڶ�λ"));
	WriteInfo(cc);

	//��ʼ�����
	m_tStartDebugTime = CTime::GetCurrentTime();
	m_tResetSystemStorage = m_tStartDebugTime;
	m_tUpdateSystemStorage = m_tStartDebugTime;
	m_lSystemStorage = m_pCustomRule->lSystemStorage;
	m_lUserStorage = m_pCustomRule->lUserStorage;
	m_nParameterK = m_pCustomRule->nParameterK;
	m_lInitSystemStorage = m_pCustomRule->lSystemStorage;
	m_lInitUserStorage = m_pCustomRule->lUserStorage;
	m_lLastInitSystemStorage = m_lInitSystemStorage;
	m_lLastInitUserStorage = m_lInitUserStorage;
	m_nInitParameterK = m_pCustomRule->nParameterK;
	m_lStorageCurrent = 0;
	m_nUpdateStorageCount = 1;
	m_wRealPlayerCount = 0;
	m_nLastThundeCount = 0;
	m_wAllPlayerCount = 0;

	//for (int n = 1; n <= 100; n++)
 //	for (int i = 0; i < 10; i++)
 //	{
 //		m_cbThunderPoint = i;
 //		for (int j = 0; j < 2; j++)
 //		{
 //			m_pCustomRule->cbThunderPosition = j;
 //			for (int k = 0; k < 10; k++)
 //			{
 //				m_pCustomRule->cbBagCount = k + 1;
 //
 //				//���ɺ��
 //				DOUBLE dBagBalance = 10.0 + n / 100.0;
	//			if (dBagBalance < m_pCustomRule->cbBagCount* 0.01)
	//			{
	//				dBagBalance = m_pCustomRule->cbBagCount* 0.01;
	//			}
 //				BYTE cbThundePointCount = 0;
	//			ZeroMemory(m_dBagScore, sizeof(m_dBagScore));
	//			ZeroMemory(m_bBagThundePoint, sizeof(m_bBagThundePoint));
 //				for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
 //				{
 //					m_bBagThundePoint[i] = GetBagDetail(NULL, dBagBalance, m_pCustomRule->cbBagCount - i, m_dBagScore[i]);
 //					if (m_bBagThundePoint[i])
 //					{
 //						cbThundePointCount++;
 //					}
 //					dBagBalance -= m_dBagScore[i];
 //				} 
	//				
	//			BYTE cbDebugThundePointCount = GetMaxThunderCount(10.0 + n / 100.0, m_cbThunderPoint, m_pCustomRule->cbThunderPosition, m_pCustomRule->cbBagCount);

 //				//��������
 //				while (cbThundePointCount != cbDebugThundePointCount)
 //				{
 //					int nRandBagIndex = rand() % m_pCustomRule->cbBagCount;
 //					if (cbThundePointCount > cbDebugThundePointCount)
 //					{
 //						for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
 //						{
 //							if (m_bBagThundePoint[i])
 //							{
	//							m_bBagThundePoint[i] = UpdateThunderInfo(false, m_dBagScore[i]);
 //								break;
 //							}
 //							nRandBagIndex = (nRandBagIndex + 1) % m_pCustomRule->cbBagCount;
 //						}
 //					}
 //					else
 //					{
 //						for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
 //						{
 //							if (!m_bBagThundePoint[i])
 //							{
	//							m_bBagThundePoint[i] = UpdateThunderInfo(true, m_dBagScore[i]);
 //								break;
 //							}
 //							nRandBagIndex = (nRandBagIndex + 1) % m_pCustomRule->cbBagCount;
 //						}
 //					}
 //
 //					//ͳ������
 //					cbThundePointCount = 0;
 //					for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
 //					{
 //						m_bBagThundePoint[i] = m_GameLogic.IsHitThunderPoint(m_dBagScore[i], m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS);
 //						if (m_bBagThundePoint[i])
 //						{
 //							cbThundePointCount++;
 //						}
 //					}
 //				}
 //			}
 //		}
 //	}

	return true;
}

//��λ����
void CTableFrameSink::RepositionSink()
{
	m_dwBeginTime = 0;
	m_wBankerUser = INVALID_CHAIR;
	m_lBagScore = 0;
	m_cbThunderPoint = INVALID_BYTE;
	m_cbAllGetCount = 0;
	m_cbAIGetCount = 0;
	m_lBagKey = 0;
	memset(m_wGetUser, INVALID_CHAIR, sizeof(m_wGetUser));
	ZeroMemory(m_dBagDetail, sizeof(m_dBagDetail));
	ZeroMemory(m_dUserScore, sizeof(m_dUserScore));
	ZeroMemory(m_bThunderPoint, sizeof(m_bThunderPoint));

	m_bDebugBag = false;
	ZeroMemory(m_bBagScore, sizeof(m_bBagScore));
	ZeroMemory(m_dBagScore, sizeof(m_dBagScore));
	ZeroMemory(m_bBagThundePoint, sizeof(m_bBagThundePoint));
	ZeroMemory(m_bDebugThunderPoint, sizeof(m_bDebugThunderPoint));
	m_dBagBalance = 0.0;
}

//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		return 0L;
	}
	else
	{
		return __max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore, 0L);
	}
}

//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge( WORD wChairID )
{
	return false;
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return true;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	RepositionSink();

	m_lSystemGameScore = 0;
	m_wBankerUser = m_vecRedBag[0].wDispatchUser;
	m_lBagScore = m_vecRedBag[0].lBagScore;	
	m_cbThunderPoint = m_vecRedBag[0].cbThunderPoint;
	m_lBagKey = m_vecRedBag[0].lBagKey;
	m_dBagBalance = (DOUBLE)m_lBagScore;
	m_cbAllGetCount = 0;
	m_cbAIGetCount = 0;

	m_lBagTotalScore[m_wBankerUser] -= m_lBagScore;
	m_vecRedBag.erase(m_vecRedBag.begin());

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
			continue;

		pIServerUserItem->SetUserStatus(US_READY, m_pITableFrame->GetTableID(), i);
	}

	if (m_wBankerUser != INVALID_CHAIR)
	{
		IServerUserItem *pIBankerUserItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
		if (pIBankerUserItem != NULL)
		{
			pIBankerUserItem->SetUserStatus(US_PLAYING, m_pITableFrame->GetTableID(), m_wBankerUser);

			//���ɺ��
			RandAllBagDetail(pIBankerUserItem);
		}
	}

	//���⴦��
	if (m_wRealPlayerCount != 1)
	{
		m_nLastThundeCount = 0;
		m_nNoThundeCount = 0;
	}

	CMD_S_GameStart gameStart;
	ZeroMemory(&gameStart, sizeof(gameStart));
	gameStart.wBankerUser = m_wBankerUser;
	gameStart.lScore = m_lBagScore;
	gameStart.cbThunderPoint = m_cbThunderPoint;
	gameStart.lBagKey = m_lBagKey;

	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_START, &gameStart, sizeof(gameStart));

	m_pITableFrame->SetGameTimer(IDI_GAME_END, m_pCustomRule->cbPlayTime * 1000, 1, NULL);
	m_dwBeginTime = (DWORD)time(NULL);

	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

	CString cc;
	cc.Format(_T("��Ϸ��ʼ,��ǰׯ��%d,������%I64d,�׵�%d,����%I64d"), m_wBankerUser, m_lBagScore, m_cbThunderPoint, m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore());
	WriteInfo(cc);

	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL://�������	
		{
			WriteInfo(_T("��������"));

			//������ʱ��
			m_pITableFrame->KillGameTimer(IDI_GAME_END);

			//���µ��Խ��
			IServerUserItem *pIBankerUserItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
			UpdateCtrlRes(pIBankerUserItem, -m_lSystemGameScore);

			//�����÷���Ϣ
			CMD_S_GameEnd gameEnd;
			ZeroMemory(&gameEnd, sizeof(gameEnd));
			gameEnd.cbGetCount = m_cbAllGetCount;
			CopyMemory(gameEnd.dBagScore, m_dBagDetail, sizeof(gameEnd.dBagScore[0])*m_cbAllGetCount);
			CopyMemory(gameEnd.bThunderPoint, m_bThunderPoint, m_cbAllGetCount);

			//�÷�
			SCORE lBankerScore = 0;
			for (int i = 0; i < m_cbAllGetCount; i++)
			{
				//��ҵ÷�
				SCORE lUserScore = (SCORE)(m_dBagDetail[i] * GAME_TIME + 0.5);
				if (m_bThunderPoint[i])
				{
					lUserScore -= (SCORE)(m_lBagScore*m_pCustomRule->dBagTimes*GAME_TIME);
				}

				if (m_wGetUser[i] != m_wBankerUser)
				{
					lBankerScore -= lUserScore;
					if (lUserScore > 0)
					{
						SCORE lRevenue = m_pITableFrame->CalculateRevenue(m_wGetUser[i], lUserScore);
						if (lRevenue > 0)
						{
							lUserScore -= lRevenue;
						}
					}
				}
				gameEnd.dUserScore[i] = lUserScore / (double)GAME_TIME;
			}

			//ׯ�ҵ÷�
			if (lBankerScore > 0)
			{
				SCORE lRevenue = m_pITableFrame->CalculateRevenue(m_wBankerUser, lBankerScore);
				if (lRevenue > 0)
				{
					lBankerScore -= lRevenue;
				}
			}
			gameEnd.dBankerScore = lBankerScore / (double)GAME_TIME;


			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_END, &gameEnd, sizeof(gameEnd));

			//д��
			lBankerScore = 0;
			for (int i = 0; i < m_cbAllGetCount; i++)
			{
				if (m_wGetUser[i] == m_wBankerUser)
					continue;

				//�����Ϸ�÷�
				SCORE lUserScore = (SCORE)(m_dBagDetail[i] * GAME_TIME + 0.5);
				if (m_bThunderPoint[i])
				{
					lUserScore -= (SCORE)(m_lBagScore*m_pCustomRule->dBagTimes*GAME_TIME);
				}

				//ׯ�ҵ÷�
				lBankerScore -= lUserScore;

				tagScoreInfo scoreinfo;
				ZeroMemory(&scoreinfo, sizeof(scoreinfo));
				scoreinfo.cbType = lUserScore > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
				if (lUserScore > 0)
				{
					scoreinfo.lRevenue = m_pITableFrame->CalculateRevenue(m_wGetUser[i], lUserScore);
				}

				//������յ÷�
				lUserScore = (SCORE)(lUserScore * m_pCustomRule->nScoreRatio / (double)GAME_TIME);
				scoreinfo.lRevenue = (SCORE)(scoreinfo.lRevenue * m_pCustomRule->nScoreRatio / (double)GAME_TIME);
				scoreinfo.lScore = lUserScore - scoreinfo.lRevenue;
				m_pITableFrame->WriteUserScore(m_wGetUser[i], scoreinfo);

				CString cc;
				cc.Format(_T("���%d��������%lf,����=%d,�ܷ�=%I64d,д��=%I64d,˰��=%I64d"), m_wGetUser[i], m_dBagDetail[i], m_bThunderPoint[i],
					lUserScore,scoreinfo.lScore,scoreinfo.lRevenue);
				WriteInfo(cc);
			}

			//ׯ����Ϸ�÷�
			tagScoreInfo scoreinfo;
			ZeroMemory(&scoreinfo, sizeof(scoreinfo));
			scoreinfo.cbType = lBankerScore > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
			if (lBankerScore > 0)
			{
				scoreinfo.lRevenue = m_pITableFrame->CalculateRevenue(m_wBankerUser, lBankerScore);
			}
			//ׯ�����յ÷�
			lBankerScore = (SCORE)(lBankerScore *m_pCustomRule->nScoreRatio / (double)GAME_TIME);
			scoreinfo.lRevenue = (SCORE)(scoreinfo.lRevenue *m_pCustomRule->nScoreRatio / (double)GAME_TIME);
			scoreinfo.lScore = lBankerScore - scoreinfo.lRevenue;
			m_pITableFrame->WriteUserScore(m_wBankerUser, scoreinfo);

			CString cc;
			cc.Format(_T("ׯ��%d���㣬�ܷ�=%I64d,д��%I64d,��˰%I64d"), m_wBankerUser,lBankerScore, scoreinfo.lScore, scoreinfo.lRevenue);
			WriteInfo(cc);

			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);


			if (m_vecRedBag.size() > 0)
			{
				m_pITableFrame->SetGameTimer(IDI_CHECK_BAG, 2000, 1, NULL);
			}

			return true;
		}
	case GER_NETWORK_ERROR:		//�����ж�
	case GER_USER_LEAVE:		//�û��뿪
		{

			return true;
		}
	}

	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:
	case GAME_SCENE_PLAY://����״̬
		{
			//��������
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));

			//������Ϣ
			StatusPlay.nScoreRatio = m_pCustomRule->nScoreRatio;
			StatusPlay.cbPlayTime = m_pCustomRule->cbPlayTime;
			StatusPlay.lScoreRangeMin = m_pCustomRule->lScoreRangeMin;
			StatusPlay.lScoreRangeMax = m_pCustomRule->lScoreRangeMax;
			StatusPlay.lDispatchCondition = StatusPlay.lScoreRangeMin;
			StatusPlay.dBagTimes = m_pCustomRule->dBagTimes;
			StatusPlay.cbThunderPositon = m_pCustomRule->cbThunderPosition;
			StatusPlay.cbBagCount = m_pCustomRule->cbBagCount;

			//��Ϸ��Ϣ
			StatusPlay.lBagKey = m_lBagKey;
			StatusPlay.wBankerUser = m_wBankerUser;
			StatusPlay.lScore = m_lBagScore;
			StatusPlay.cbThunderPoint = m_cbThunderPoint;
			StatusPlay.cbLeftCount = m_pCustomRule->cbBagCount- m_cbAllGetCount;
			CopyMemory(StatusPlay.wGetUser, m_wGetUser, sizeof(StatusPlay.wGetUser));
			StatusPlay.dBagDetail = 0;
			StatusPlay.bThunderPoint = false;
			for (int i = 0; i < m_cbAllGetCount; i++)
			{
				if (m_wGetUser[i] == wChairID)
				{
					StatusPlay.dBagDetail = m_dBagDetail[i];
					StatusPlay.dUserScore = m_dUserScore[i];
					StatusPlay.bThunderPoint = m_bThunderPoint[i];
					break;
				}
			}

			DWORD dwPassTime = (DWORD)time(NULL) - m_dwBeginTime;
			StatusPlay.cbLeftTime = (BYTE)(m_pCustomRule->cbPlayTime - __min(dwPassTime, m_pCustomRule->cbPlayTime));
			bool bRes = false;
			if (!pIServerUserItem->IsAndroidUser())
			{
				bRes = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
			}
			else
			{
				//��������
				CMD_S_StatusPlayAI StatusPlayAI;
				CopyMemory(&StatusPlayAI.StatusPlay, &StatusPlay, sizeof(StatusPlayAI.StatusPlay));

				//AI����
				StatusPlayAI.cbMinBagListCount = m_pCustomRule->cbMinBagListCount;;
				StatusPlayAI.cbMaxRealBagListCount = m_pCustomRule->cbMaxRealBagListCount;
				CopyMemory(StatusPlayAI.cbSetBagCount, m_pCustomRule->cbSetBagCount, sizeof(StatusPlayAI.cbSetBagCount));
				CopyMemory(StatusPlayAI.cbSetBagTime, m_pCustomRule->cbSetBagTime, sizeof(StatusPlayAI.cbSetBagTime));
				CopyMemory(StatusPlayAI.lSetBagScore, m_pCustomRule->lSetBagScore, sizeof(StatusPlayAI.lSetBagScore));
				StatusPlayAI.cbStartGetBagTime = m_pCustomRule->cbStartGetBagTime;
				CopyMemory(StatusPlayAI.cbGetBagTime, m_pCustomRule->cbGetBagTime, sizeof(StatusPlayAI.cbGetBagTime));
				StatusPlayAI.cbMaxGetBagCount = m_pCustomRule->cbMaxGetBagCount;
				StatusPlayAI.cbLastNoGetBagCount = m_pCustomRule->cbLastNoGetBagCount;
				bRes = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlayAI, sizeof(StatusPlayAI));
			}
			if (bRes)
			{
				SendBagList(wChairID);
			}

			return bRes;
		}
	}
	
	return false;
}

//��ʱ���¼�
bool  CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)
{
	switch (dwTimerID)
	{
	case  IDI_CHECK_BAG:
		{
			for (UINT i = 0; i < m_vecRedBag.size();)
			{
				IServerUserItem *pUser = m_pITableFrame->GetTableUserItem(m_vecRedBag[i].wDispatchUser);
				if (pUser == NULL || pUser->GetUserScore() < m_vecRedBag[i].lBagScore*m_pCustomRule->nScoreRatio)
				{
					CMD_S_CancelBag CancelBag;
					CancelBag.lBagKey = m_vecRedBag[i].lBagKey;
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BAG, &CancelBag, sizeof(CancelBag));
					CString cc;
					cc.Format(_T("����������ɾ�����%d�����key=%I64d"), m_vecRedBag[i].wDispatchUser, CancelBag.lBagKey);
					WriteInfo(cc);
					m_vecRedBag.erase(m_vecRedBag.begin() + i);
				}
				else
				{
					i++;
				}
			}

			if (m_wBankerUser == INVALID_CHAIR && m_vecRedBag.size() > 0)
			{
				m_pITableFrame->StartGame();
			}

			return true;
		}
	case IDI_GAME_END:		//��������
		{
			WriteInfo(_T("���������ʱ����"));
			OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

			return true;
		}
	}
	
	return true;
}

//��Ϸ��Ϣ����
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_DISPATCH_BAG:				//�����
		{
			//Ч������
			ASSERT (wDataSize == sizeof(CMD_C_DispatchBag));
			if (wDataSize != sizeof(CMD_C_DispatchBag))
			{
				return false;
			}

			//��Ϣ����
			CMD_C_DispatchBag *pBag = (CMD_C_DispatchBag *)pData;
			return OnUserDispatchBag(pIServerUserItem->GetChairID(), pBag->lScore, (BYTE)pBag->lThunderPoint);
		}
	case SUB_C_GET_BAG:
		{
			//Ч������
			ASSERT (wDataSize == sizeof(CMD_C_GetBag));
			if (wDataSize != sizeof(CMD_C_GetBag))
			{
				return false;
			}

			//��Ϣ����
			CMD_C_GetBag *pGet = (CMD_C_GetBag *)pData;
			return OnUserGetBag(pIServerUserItem->GetChairID(), pGet->lBagKey);

		}
	}

	return true;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// ��Ϣ����
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		// �ж�Ȩ��
		if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		{
			return false;
		}

		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
		case SUB_C_REFRESH_STORAGE:					//ˢ�¿��
		{
			//Ч������
			ASSERT(wDataSize == 0);
			if (wDataSize != 0) return false;

			//���Ϳ����Ϣ
			SendBaseStorageInfo(pIServerUserItem);

			//������ҵ���
			SendUserDebugInfo(pIServerUserItem);

			//���ͷ������
			SendRoomDebugInfo(pIServerUserItem);

			//����ϵͳ����
			SendSysDebugInfo(pIServerUserItem);

			return true;
		}
		case SUB_C_SET_CUR_STORAGE:		//���õ�ǰ���
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_SetCurStorage));
			if (wDataSize != sizeof(CMD_C_SetCurStorage)) return false;

			//��������
			CMD_C_SetCurStorage * pSetCurStorage = (CMD_C_SetCurStorage *)pData;


			//�����¼
			tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
			HistoryRoomDebugInfo.dwDebugIndex = m_dwSysDebugIndex;
			HistoryRoomDebugInfo.bSystemStorageEnd = m_bSystemStorageEnd;
			HistoryRoomDebugInfo.lDebugTime = m_tResetSystemStorage.GetTime();
			HistoryRoomDebugInfo.lInitSystemStorage = m_lLastInitSystemStorage;
			HistoryRoomDebugInfo.lSystemStorage = m_lSystemStorage;
			HistoryRoomDebugInfo.lInitUserStorage = m_lLastInitUserStorage;
			HistoryRoomDebugInfo.lUserStorage = m_lUserStorage;
			HistoryRoomDebugInfo.nParameterK = m_nParameterK;
			HistoryRoomDebugInfo.nDebugStatus = CANCEL_STATUS;
			m_listHistorySysDebugInfo.AddTail(HistoryRoomDebugInfo);
			if (m_listHistorySysDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
			{
				m_listHistorySysDebugInfo.RemoveHead();
			}

			//���Ϳ�����
			SendSysDebugInfo(m_dwSysDebugIndex, m_lSystemStorage, m_lUserStorage, CANCEL_STATUS, m_tUpdateSystemStorage.GetTime());

			m_tResetSystemStorage = CTime::GetCurrentTime();
			m_dwSysDebugIndex++;
			m_nUpdateStorageCount++;
			m_lSystemStorage = pSetCurStorage->lSystemStorage;
			m_lUserStorage = pSetCurStorage->lUserStorage;
			m_nParameterK = pSetCurStorage->nParameterK;
			m_lInitSystemStorage = pSetCurStorage->lSystemStorage;
			m_lLastInitSystemStorage = pSetCurStorage->lSystemStorage;
			m_lInitUserStorage = pSetCurStorage->lUserStorage;
			m_lLastInitUserStorage = pSetCurStorage->lUserStorage;
			m_nInitParameterK = pSetCurStorage->nParameterK;
			m_bSystemStorageEnd = pSetCurStorage->bSystemStorageEnd;

			//���Ϳ����Ϣ
			SendBaseStorageInfo(pIServerUserItem);

			//������ʾ
			SendDebugText(pIServerUserItem, TEXT("���õ�ǰ���ɹ�"));

			return true;
		}
		case SUB_C_SET_ROOM_STORAGE:		//���÷�����
		{
			ASSERT(wDataSize == sizeof(CMD_C_SetRoomStorage));
			if (wDataSize != sizeof(CMD_C_SetRoomStorage))
			{
				return false;
			}

			CMD_C_SetRoomStorage *pSetRoomStorage = (CMD_C_SetRoomStorage *)pData;

			
			if (pSetRoomStorage->lSystemStorage < 0 || pSetRoomStorage->lUserStorage < 0 ||
				pSetRoomStorage->lSystemStorage <= 0 && pSetRoomStorage->lUserStorage <= 0 ||
				pSetRoomStorage->nParameterK <= 0 || pSetRoomStorage->nParameterK >= 100)
			{
				//������ʾ
				SendDebugText(pIServerUserItem, TEXT("�޸ķ��������ʧ�ܣ���������Ч��"));
				return true;
			}
		
			//���ò���
			tagRoomDebugInfo RoomDebugInfo;
			ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
			RoomDebugInfo.tDebugTime = CTime::GetCurrentTime();
			RoomDebugInfo.tUpdateTime = RoomDebugInfo.tDebugTime;
			RoomDebugInfo.dwDebugIndex = m_dwRoomDebugIndex++;
			RoomDebugInfo.lInitSystemStorage = pSetRoomStorage->lSystemStorage;
			RoomDebugInfo.lSystemStorage = pSetRoomStorage->lSystemStorage;
			RoomDebugInfo.lInitUserStorage = pSetRoomStorage->lUserStorage;
			RoomDebugInfo.lUserStorage = pSetRoomStorage->lUserStorage;
			RoomDebugInfo.nParameterK = pSetRoomStorage->nParameterK;
			RoomDebugInfo.bSystemStorageEnd = pSetRoomStorage->bSystemStorageEnd;
			RoomDebugInfo.nDebugStatus = m_listRoomDebugInfo.GetCount() == 0 ? RUNNNING_STATUS : WIAT_STATUS;
			m_listRoomDebugInfo.AddTail(RoomDebugInfo);		

			//�������
			CMD_S_SetRoomStorageInfo SetRoomStorageInfo;
			SetRoomStorageInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
			SetRoomStorageInfo.lSystemStorage = RoomDebugInfo.lSystemStorage;
			SetRoomStorageInfo.lUserStorage = RoomDebugInfo.lUserStorage;
			SetRoomStorageInfo.nParameterK = RoomDebugInfo.nParameterK;
			SetRoomStorageInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
			SetRoomStorageInfo.lUpdateTime = RoomDebugInfo.tDebugTime.GetTime();
			SetRoomStorageInfo.nDebugStatus = RoomDebugInfo.nDebugStatus;

			// ������Ϣ
			m_pITableFrame->SendRoomData(NULL, SUB_S_SET_ROOM_STORAGE, &SetRoomStorageInfo, sizeof(SetRoomStorageInfo));

			//������ʾ
			SendDebugText(pIServerUserItem, TEXT("���÷�����Գɹ�"));

			return true;
		}
		case SUB_C_CANCEL_ROOM_STORAGE:				//ȡ��������
		{
			ASSERT(wDataSize == sizeof(CMD_C_CancelRoomStorage));
			if (wDataSize != sizeof(CMD_C_CancelRoomStorage))
			{
				return false;
			}

			CMD_C_CancelRoomStorage *pCancelRoomSStorage = (CMD_C_CancelRoomStorage *)pData;

			//�����Ѵ��ڵķ������;
			if (!DelFirstRoomDebugInfo(pCancelRoomSStorage->dwDebugIndex))
			{
				//������ʾ
				SendDebugText(pIServerUserItem, TEXT("ɾ�����������ʧ�ܣ����� %d ������"), pCancelRoomSStorage->dwDebugIndex);
				return true;
			}

			//������ʾ
			SendDebugText(pIServerUserItem, TEXT("ȡ��������Գɹ�"));
			return true;
		}
		//case SUB_C_SET_USER_STORAGE:		//������ҿ��
		//{
		//	ASSERT(wDataSize == sizeof(CMD_C_SetUserStorage));
		//	if (wDataSize != sizeof(CMD_C_SetUserStorage))
		//	{
		//		return false;
		//	}

		//	CMD_C_SetUserStorage *pSetUserStorage = (CMD_C_SetUserStorage *)pData;

		//	if (pSetUserStorage->lSystemStorage < 0 || pSetUserStorage->lUserStorage < 0 ||
		//		pSetUserStorage->lSystemStorage <= 0 && pSetUserStorage->lUserStorage <= 0 ||
		//		pSetUserStorage->nParameterK <= 0 || pSetUserStorage->nParameterK >= 100)
		//	{
		//		//������ʾ
		//		SendDebugText(pIServerUserItem, TEXT("�޸���ҿ�����ʧ�ܣ���������Ч��"));
		//		return true;
		//	}

		//	tagRoomUserInfo RoomUserInfo;
		//	ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));

		//	//�����Ѵ��ڵķ������;
		//	if (!m_mapRoomUserInfo.Lookup(pSetUserStorage->dwGameID, RoomUserInfo))
		//	{
		//		//������ʾ
		//		SendDebugText(pIServerUserItem, TEXT("�޸���ҿ�����ʧ�ܣ�GameID %d �����ڻ�����Ϸ�У�"), pSetUserStorage->dwGameID);
		//		return true;
		//	}
		//	CTime tDebugTime = CTime::GetCurrentTime();
		//	if (RoomUserInfo.bDebugIndividual)
		//	{
		//		if (RoomUserInfo.lInitSystemStorage == RoomUserInfo.lSystemStorage && RoomUserInfo.lInitUserStorage == RoomUserInfo.lUserStorage)
		//		{
		//			//�������
		//			CMD_S_DeleteStorageInfo DeleteStorageInfo;
		//			DeleteStorageInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;

		//			// ������Ϣ
		//			m_pITableFrame->SendRoomData(NULL, SUB_S_DEL_USER_STORAGE, &DeleteStorageInfo, sizeof(DeleteStorageInfo));
		//		}
		//		else
		//		{
		//			//�����¼
		//			tagHistoryRoomUserInfo HistoryRoomUserInfo;
		//			HistoryRoomUserInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
		//			HistoryRoomUserInfo.dwGameID = pIServerUserItem->GetGameID();
		//			HistoryRoomUserInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
		//			HistoryRoomUserInfo.lDebugTime = RoomUserInfo.tDebugTime.GetTime();
		//			HistoryRoomUserInfo.lUpdateTime = tDebugTime.GetTime();
		//			HistoryRoomUserInfo.lInitSystemStorage = RoomUserInfo.lInitSystemStorage;
		//			HistoryRoomUserInfo.lSystemStorage = RoomUserInfo.lSystemStorage;
		//			HistoryRoomUserInfo.lInitUserStorage = RoomUserInfo.lInitUserStorage;
		//			HistoryRoomUserInfo.lUserStorage = RoomUserInfo.lUserStorage;
		//			HistoryRoomUserInfo.nParameterK = RoomUserInfo.nParameterK;
		//			HistoryRoomUserInfo.nDebugStatus = CANCEL_STATUS;
		//			m_listHistoryRoomUserInfo.AddTail(HistoryRoomUserInfo);
		//			if (m_listHistoryRoomUserInfo.GetCount() > HISTORY_DEBUG_COUNT)
		//			{
		//				m_listHistoryRoomUserInfo.RemoveHead();
		//			}

		//			//���Ϳ�����
		//			SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.lSystemStorage, RoomUserInfo.lUserStorage, CANCEL_STATUS, RoomUserInfo.tUpdateTime.GetTime());
		//		}
		//	}

		//	//���ò���
		//	RoomUserInfo.bDebugIndividual = true;
		//	RoomUserInfo.dwDebugIndex = m_dwUserDebugIndex++;
		//	RoomUserInfo.tDebugTime = tDebugTime;
		//	RoomUserInfo.tUpdateTime = RoomUserInfo.tDebugTime;
		//	RoomUserInfo.lInitSystemStorage = pSetUserStorage->lSystemStorage;
		//	RoomUserInfo.lSystemStorage = pSetUserStorage->lSystemStorage;
		//	RoomUserInfo.lInitUserStorage = pSetUserStorage->lUserStorage;
		//	RoomUserInfo.lUserStorage = pSetUserStorage->lUserStorage;
		//	RoomUserInfo.nParameterK = pSetUserStorage->nParameterK;
		//	RoomUserInfo.bSystemStorageEnd = pSetUserStorage->bSystemStorageEnd;
		//	m_mapRoomUserInfo.SetAt(pSetUserStorage->dwGameID, RoomUserInfo);

		//	//�������
		//	CMD_S_SetUserStorageInfo SetUserStorageInfo;
		//	SetUserStorageInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
		//	SetUserStorageInfo.dwGameID = pSetUserStorage->dwGameID;
		//	SetUserStorageInfo.lSystemStorage = RoomUserInfo.lSystemStorage;
		//	SetUserStorageInfo.lUserStorage = RoomUserInfo.lUserStorage;
		//	SetUserStorageInfo.nParameterK = RoomUserInfo.nParameterK;
		//	SetUserStorageInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
		//	SetUserStorageInfo.lUpdateTime = RoomUserInfo.tDebugTime.GetTime();

		//	// ������Ϣ
		//	m_pITableFrame->SendRoomData(NULL, SUB_S_SET_USER_STORAGE, &SetUserStorageInfo, sizeof(SetUserStorageInfo));

		//	//������ʾ
		//	SendDebugText(pIServerUserItem, TEXT("������ҵ��Գɹ�"));

		//	return true;
		//}
		//case SUB_C_CANCEL_USER_STORAGE:				//ȡ����ҿ��
		//{
		//	ASSERT(wDataSize == sizeof(CMD_C_CancelUserStorage));
		//	if (wDataSize != sizeof(CMD_C_CancelUserStorage))
		//	{
		//		return false;
		//	}

		//	CMD_C_CancelUserStorage *pCancelUserStorage = (CMD_C_CancelUserStorage *)pData;

		//	tagRoomUserInfo RoomUserInfo;
		//	ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));

		//	//�����Ѵ��ڵķ������;
		//	if (!m_mapRoomUserInfo.Lookup(pCancelUserStorage->dwGameID, RoomUserInfo))
		//	{
		//		//������ʾ
		//		SendDebugText(pIServerUserItem, TEXT("ɾ����ҿ�����ʧ�ܣ�GameID %d ��Ч��"), pCancelUserStorage->dwGameID);
		//		return true;
		//	}

		//	if (pCancelUserStorage->dwDebugIndex != RoomUserInfo.dwDebugIndex)
		//	{
		//		//������ʾ
		//		SendDebugText(pIServerUserItem, TEXT("ɾ����ҿ�����ʧ�ܣ�����ID %d ��Ч��"), pCancelUserStorage->dwDebugIndex);
		//		return true;
		//	}

		//	//�����Ѵ��ڵķ������;
		//	if (!RoomUserInfo.bDebugIndividual)
		//	{
		//		//������ʾ
		//		SendDebugText(pIServerUserItem, TEXT("ɾ����ҿ�����ʧ�ܣ�GameID %d �޵��ԣ�"), pCancelUserStorage->dwGameID);
		//		return true;
		//	}


		//	//���ò���
		//	RoomUserInfo.bDebugIndividual = false;
		//	m_mapRoomUserInfo.SetAt(pCancelUserStorage->dwGameID, RoomUserInfo);

		//	if (RoomUserInfo.lInitSystemStorage != RoomUserInfo.lSystemStorage || RoomUserInfo.lInitUserStorage != RoomUserInfo.lUserStorage)
		//	{
		//		//�����¼
		//		tagHistoryRoomUserInfo HistoryRoomUserInfo;
		//		HistoryRoomUserInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
		//		HistoryRoomUserInfo.dwGameID = pIServerUserItem->GetGameID();
		//		HistoryRoomUserInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
		//		HistoryRoomUserInfo.lDebugTime = RoomUserInfo.tDebugTime.GetTime();
		//		HistoryRoomUserInfo.lInitSystemStorage = RoomUserInfo.lInitSystemStorage;
		//		HistoryRoomUserInfo.lSystemStorage = RoomUserInfo.lSystemStorage;
		//		HistoryRoomUserInfo.lInitUserStorage = RoomUserInfo.lInitUserStorage;
		//		HistoryRoomUserInfo.lUserStorage = RoomUserInfo.lUserStorage;
		//		HistoryRoomUserInfo.nParameterK = RoomUserInfo.nParameterK;
		//		HistoryRoomUserInfo.nDebugStatus = CANCEL_STATUS;
		//		m_listHistoryRoomUserInfo.AddTail(HistoryRoomUserInfo);
		//		if (m_listHistoryRoomUserInfo.GetCount() > HISTORY_DEBUG_COUNT)
		//		{
		//			m_listHistoryRoomUserInfo.RemoveHead();
		//		}

		//		//���Ϳ�����
		//		SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.lSystemStorage, RoomUserInfo.lUserStorage, CANCEL_STATUS, RoomUserInfo.tUpdateTime.GetTime());
		//	}
		//	else
		//	{
		//		//�������
		//		CMD_S_DeleteStorageInfo DeleteStorageInfo;
		//		DeleteStorageInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;

		//		// ������Ϣ
		//		m_pITableFrame->SendRoomData(NULL, SUB_S_DEL_USER_STORAGE, &DeleteStorageInfo, sizeof(DeleteStorageInfo));
		//	}

		//	//������ʾ
		//	SendDebugText(pIServerUserItem, TEXT("ȡ����ҵ��Գɹ�"));

		//	return true;
		//}
		}
	}
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	CString cc;
	cc.Format(_T("���%d���ߣ�Я������%I64d,gameid=%d,name=%s"), wChairID, pIServerUserItem->GetUserScore(), pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName());
	WriteInfo(cc);

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{

	CString cc;
	cc.Format(_T("���%d���£�Я������%I64d,gameid=%d,name=%s,ai=%d"), wChairID, pIServerUserItem->GetUserScore(), pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	WriteInfo(cc);

	m_lBagTotalScore[wChairID] = 0;

	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	//ͳ�����
	m_wAllPlayerCount++;
	if (!pIServerUserItem->IsAndroidUser())
	{
		m_wRealPlayerCount++;
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	CString cc;
	cc.Format(_T("���%d������Я������%I64d,gameid=%d,name=%s"), wChairID, pIServerUserItem->GetUserScore(), pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName());
	WriteInfo(cc);

	//ɾ���ŶӺ���б�
	for (UINT i = 0; i < m_vecRedBag.size();)
	{
		if (m_vecRedBag[i].wDispatchUser == wChairID)
		{
			CMD_S_CancelBag CancelBag;
			CancelBag.lBagKey = m_vecRedBag[i].lBagKey;

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BAG, &CancelBag, sizeof(CancelBag));
			cc.Format(_T("�����뿪��ɾ�����%d�����key=%I64d"), wChairID, CancelBag.lBagKey);
			WriteInfo(cc);

			m_vecRedBag.erase(m_vecRedBag.begin() + i);
		}
		else
		{
			i++;
		}
	}

	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

	//ͳ���������
	m_wAllPlayerCount--;
	if (!pIServerUserItem->IsAndroidUser())
	{
		m_wRealPlayerCount--;
	}

	return true;
}


void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{
	//���� 2020/01/05 ���λ�������־
#ifndef DEBUG
	return;
#endif // !DEBUG
	//������������
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	//����Ŀ¼
	TCHAR szFilePack[MAX_PATH];
	GetModuleFileName(NULL, szFilePack, MAX_PATH);
	(wcsrchr(szFilePack, '\\'))[0] = 0;
	_stprintf_s(szFilePack, _T("%s\\ɨ�׺����־Ŀ¼"), szFilePack);

	DWORD dwAttrib = GetFileAttributes(szFilePack);
	if (INVALID_FILE_ATTRIBUTES == dwAttrib)
	{
		CreateDirectory(szFilePack, NULL);
	}

	//�����ļ�
	CTime time = CTime::GetCurrentTime();
	static CString strFile;
	if (strFile.IsEmpty())
		strFile.Format(_T("%s\\[%s]��־%d-%d-%d-%02d%02d%02d.log"), szFilePack, m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	//д�ļ�
	CStdioFile myFile;
	BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		if (myFile.GetLength() > 50 * 1024 * 1024)//50M���ļ� 
		{
			myFile.Close();
			strFile.Format(_T("%s\\[%s]��־%d-%d-%d-%02d%02d%02d.log"), szFilePack, m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
			if (bOpen == false)
				return;
		}

		CString strtip;
		strtip.Format(TEXT("�����%d,%d/%d/%d_%d:%d:%d--%s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pszString);

		myFile.SeekToEnd();
		myFile.WriteString(strtip);
		myFile.Flush();
		myFile.Close();
	}

	//��ԭ�����趨
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}

//��ҷ����
bool CTableFrameSink::OnUserDispatchBag(WORD wChairID, LONGLONG lScore, BYTE cbThunderPoint)
{
	IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	ASSERT(pIUserItem != NULL);

	if (pIUserItem == NULL) return true;

	LONGLONG lBagKey = MAKELONGLONG(GetTickCount(), m_pITableFrame->GetTableUserItem(wChairID)->GetUserID());
	CString cc;
	cc.Format(_T("���%d����� score=%I64d,Thunder=%d,Key=%I64d"), wChairID, lScore, cbThunderPoint,lBagKey);
	WriteInfo(cc);

	//У�鷢�����С
	ASSERT(lScore >= m_pCustomRule->lScoreRangeMin && lScore <= m_pCustomRule->lScoreRangeMax );
	if (lScore < m_pCustomRule->lScoreRangeMin || lScore > m_pCustomRule->lScoreRangeMax)
	{
		cc.Format(_T("���ĺ��С��%I64d����%I64d�����ܷ������"), m_pCustomRule->lScoreRangeMin, m_pCustomRule->lScoreRangeMax);
		m_pITableFrame->SendGameMessage(pIUserItem, cc, SMT_CHAT | SMT_EJECT);
		return true;
	}

	/*
	//У�鷢�������
	ASSERT(pIUserItem->GetUserScore() >= m_pCustomRule->nScoreRatio * m_pCustomRule->lDispatchCondition);
	if (pIUserItem->GetUserScore() < m_pCustomRule->nScoreRatio * m_pCustomRule->lDispatchCondition)
	{
		cc.Format(_T("���ķ���С��%I64d�����ܷ������"), m_pCustomRule->nScoreRatio * m_pCustomRule->lDispatchCondition);
		m_pITableFrame->SendGameMessage(pIUserItem, cc, SMT_CHAT | SMT_EJECT);
		return true;
	}
	*/

	//У����
	ASSERT(pIUserItem->GetUserScore() - m_lBagTotalScore[wChairID] >= m_pCustomRule->nScoreRatio * lScore);
	if (pIUserItem->GetUserScore() - m_lBagTotalScore[wChairID] < m_pCustomRule->nScoreRatio * lScore)
	{
		cc.Format(_T("����ʧ�ܣ����Ľ�Ҳ��㡣"));
		m_pITableFrame->SendGameMessage(pIUserItem, cc, SMT_CHAT | SMT_EJECT);
		WriteInfo(cc);
		return true;
	}

	m_lBagTotalScore[wChairID] += lScore;
	
	//�����б�
	stRedBagInfo redBag;
	ZeroMemory(&redBag, sizeof(redBag));
	redBag.wDispatchUser = wChairID;
	redBag.lBagScore = lScore;
	redBag.cbThunderPoint = cbThunderPoint;
	redBag.lBagKey = lBagKey;

	bool bInsert = false;

	for (UINT i = 0; i < m_vecRedBag.size(); i++)
	{
		if (m_vecRedBag[i].lBagScore < redBag.lBagScore)
		{
			m_vecRedBag.insert(m_vecRedBag.begin() + i, redBag);
			bInsert = true;
			break;
		}
	}

	if (bInsert==false)
		m_vecRedBag.push_back(redBag);

	//������Ϣ
	CMD_S_DispatchBag dispatchBag;
	ZeroMemory(&dispatchBag, sizeof(dispatchBag));
	dispatchBag.wDispatchUser = wChairID;
	dispatchBag.lScore = lScore;
	dispatchBag.lBagKey = redBag.lBagKey;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_DISPATCH_BAG, &dispatchBag, sizeof(dispatchBag));

	if (m_vecRedBag.size() == 1 && m_wBankerUser==INVALID_CHAIR)
	{
		m_pITableFrame->SetGameTimer(IDI_CHECK_BAG, 1000, 1, NULL);
	}
	return true;
}

bool CTableFrameSink::OnUserGetBag(WORD wChairID, LONGLONG lBagKey)
{
	CString cc;
	cc.Format(_T("���%d��������%I64d"), wChairID, lBagKey);
	WriteInfo(cc);


	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY)
	{
		CMD_S_GetBag_Fail getFail;
		getFail.wGetUser = wChairID;
		getFail.cbReason = 2;
		m_pITableFrame->SendTableData(wChairID, SUB_S_GET_BAG_FAIL, &getFail, sizeof(getFail));
		WriteInfo(_T("��ǰ���������״̬"));
		return true;
	}
		

	IServerUserItem* pGetBagUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (pGetBagUserItem == NULL)
		return true;

	if (m_wBankerUser != INVALID_CHAIR)
	{
		IServerUserItem* pBankerUserItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
		if (pBankerUserItem == NULL)
			return true;
	}
	else
	{
		return true;
	}

	//�����key
	if (m_lBagKey != lBagKey)
	{
		CMD_S_GetBag_Fail getFail;
		getFail.wGetUser = wChairID;
		getFail.cbReason = 0;
		m_pITableFrame->SendTableData(wChairID, SUB_S_GET_BAG_FAIL, &getFail, sizeof(getFail));
		WriteInfo(_T("���keyֵ����"));
		return true;
	}

	//���������ҷ���
	if (pGetBagUserItem->GetUserScore() - m_lBagTotalScore[wChairID] < m_pCustomRule->dBagTimes*m_lBagScore*m_pCustomRule->nScoreRatio)
	{
		CMD_S_GetBag_Fail getFail;
		getFail.wGetUser = wChairID;
		getFail.cbReason = 1;
		m_pITableFrame->SendTableData(wChairID, SUB_S_GET_BAG_FAIL, &getFail, sizeof(getFail));
		cc.Format(_T("��ҷ���%I64d��������"), pGetBagUserItem->GetUserScore());
		WriteInfo(cc);
		return true;
	}

	//���ʣ���������
	if (m_cbAllGetCount == m_pCustomRule->cbBagCount)
	{
		CMD_S_GetBag_Fail getFail;
		getFail.wGetUser = wChairID;
		getFail.cbReason = 2;
		m_pITableFrame->SendTableData(wChairID, SUB_S_GET_BAG_FAIL, &getFail, sizeof(getFail));
		WriteInfo(_T("����ѱ�����"));
		return true;
	}

	//����ظ�����
	for (int i = 0; i < m_cbAllGetCount; i++)
	{
		if (m_wGetUser[i] == wChairID)
		{
			CMD_S_GetBag_Fail getFail;
			getFail.wGetUser = wChairID;
			getFail.cbReason = 3;
			m_pITableFrame->SendTableData(wChairID, SUB_S_GET_BAG_FAIL, &getFail, sizeof(getFail));
			cc.Format(_T("���%d�ظ�����,��ǰ��������%d"), wChairID,m_cbAllGetCount);
			for (int j = 0; j < m_cbAllGetCount; j++)
				cc.AppendFormat(_T("%d,"), m_wGetUser[j]);
			WriteInfo(cc);
			return true;
		}
	}

	//���ñ���
	m_wGetUser[m_cbAllGetCount] = wChairID;
	m_bThunderPoint[m_cbAllGetCount] = GetBagDetail(pGetBagUserItem, m_dBagDetail[m_cbAllGetCount]);
	m_dUserScore[m_cbAllGetCount] = GetBagUserScore(wChairID, m_dBagDetail[m_cbAllGetCount], m_bThunderPoint[m_cbAllGetCount]);
	m_dBagBalance -= m_dBagDetail[m_cbAllGetCount];	
	pGetBagUserItem->SetUserStatus(US_PLAYING, m_pITableFrame->GetTableID(), wChairID);

	CMD_S_GetBag getBag;
	getBag.wGetUser = wChairID;
	getBag.dBagScore = m_dBagDetail[m_cbAllGetCount];
	getBag.dUserScore = m_dUserScore[m_cbAllGetCount];
	getBag.bThunderPoint = m_bThunderPoint[m_cbAllGetCount];
	getBag.cbLeftCount = m_pCustomRule->cbBagCount - m_cbAllGetCount - 1;

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (i == wChairID)
		{
			getBag.dBagScore = m_dBagDetail[m_cbAllGetCount];
			getBag.dUserScore = m_dUserScore[m_cbAllGetCount];
			getBag.bThunderPoint = m_bThunderPoint[m_cbAllGetCount];
		}
		else
		{
			getBag.dBagScore = 0;
			getBag.dUserScore = 0;
			getBag.bThunderPoint = 0;
		}
		m_pITableFrame->SendTableData(i, SUB_S_GET_BAG, &getBag, sizeof(getBag));
	}

	//���⴦��
	if (m_wRealPlayerCount == 1 && !pGetBagUserItem->IsAndroidUser())
	{
		if (m_bThunderPoint[m_cbAllGetCount])
		{
			m_nLastThundeCount++;
		}
		else
		{
			m_nNoThundeCount--;
			m_nLastThundeCount = 0;
		}
	}

	//������
	bool bGetBagAI = pGetBagUserItem->IsAndroidUser();
	IServerUserItem *pIBankerUserItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
	bool bSetBagAI = pIBankerUserItem->IsAndroidUser();
	if (bGetBagAI != bSetBagAI)
	{
		//��ҵ÷�
		SCORE lUserScore = (SCORE)(m_dBagDetail[m_cbAllGetCount] * m_pCustomRule->nScoreRatio + 0.5);

		//����
		if (m_bThunderPoint[m_cbAllGetCount])
		{
			lUserScore -= (SCORE)(m_lBagScore * m_pCustomRule->dBagTimes * m_pCustomRule->nScoreRatio);
		}

		//���µ��Խ��
		if (bGetBagAI)
		{
			m_lSystemGameScore += lUserScore;
		}
		else
		{
			m_lSystemGameScore -= lUserScore;
		}
	}
	

	m_cbAllGetCount++;
	if (pGetBagUserItem->IsAndroidUser())
	{
		m_cbAIGetCount++;
	}
	
	cc.Format(_T("���%d�����ɹ�,ʣ��%d��"), wChairID, m_pCustomRule->cbBagCount - m_cbAllGetCount);
	WriteInfo(cc);

	if (m_cbAllGetCount == m_pCustomRule->cbBagCount)//���������
	{
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
	}
	return true;
}

//����б�
void CTableFrameSink::SendBagList(WORD wChairID)
{
	for (UINT i = 0; i < m_vecRedBag.size(); i++)
	{
		CMD_S_DispatchBag DispatchBag;
		DispatchBag.wDispatchUser = m_vecRedBag[i].wDispatchUser;
		DispatchBag.lScore = m_vecRedBag[i].lBagScore;
		DispatchBag.lBagKey = m_vecRedBag[i].lBagKey;
		m_pITableFrame->SendTableData(wChairID, SUB_S_DISPATCH_BAG, &DispatchBag, sizeof(DispatchBag));
	}
}

//���������
BYTE CTableFrameSink::GetMaxThunderCount(DOUBLE dBagScore, BYTE cbThunderPoint, BYTE cbThunderPosition, BYTE cbBagCount)
{
	BYTE cbMaxThunderCount = cbBagCount;
	DOUBLE dTimes = cbThunderPosition == 0 ? MIN_POINT_ONE : MIN_POINT_TWO;

	while (D_COMPARE(dTimes * cbBagCount + (cbBagCount - 1) * MIN_POINT_TWO, dBagScore) && cbMaxThunderCount > 0)
	{
		cbMaxThunderCount--;
	}

	return cbMaxThunderCount;
}

//���ɺ��
bool CTableFrameSink::RandAllBagDetail(IServerUserItem *pIBankerUserItem)
{
	//���ɺ��
	DOUBLE dBagBalance = (DOUBLE)m_lBagScore;
	BYTE cbThundePointCount = 0;
	for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
	{
		m_bBagThundePoint[i] = GetBagDetail(dBagBalance, m_pCustomRule->cbBagCount - i, m_dBagScore[i]);
		if (m_bBagThundePoint[i])
		{
			cbThundePointCount++;
		}
		dBagBalance -= m_dBagScore[i];
	}

	//���ɺ����������Ϣ
	if (m_wRealPlayerCount == 0 || m_wAllPlayerCount == m_wRealPlayerCount)
	{
		m_emCurCtrlType = RAND_DEBUG;
		m_bDebugBag = false;
		return false;
	}
	else
	{
		m_bDebugBag = true;

		//�������׸���
		int nThundeCount[MAX_BAG_COUNT + 1] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		int nThundeChance[MAX_BAG_COUNT + 1] = { 35, 35, 15, 10, 5, 0, 0, 0, 0, 0, 0 };
		LONGLONG lSystemStorage = 0, lUserStorage = 0;
		WORD wReturnPer = GetReturnPer(pIBankerUserItem, lSystemStorage, lUserStorage);
		m_emCurCtrlType = GetUserCtrlType(pIBankerUserItem);
		bool bIsUserWin = (rand() % 100) < wReturnPer;
		int nStartThundeIndex = 0;
		int nMaxThundeCount = 2;

		if (pIBankerUserItem->IsAndroidUser() && !bIsUserWin)
		{
			nStartThundeIndex = 1;
			if (wReturnPer >= 50)
			{
				nMaxThundeCount = 4 * m_pCustomRule->cbBagCount / 10 - (wReturnPer - 50) / 10;
			}
			else
			{
				nMaxThundeCount = 4 * m_pCustomRule->cbBagCount / 10 + (50 - wReturnPer) / 10;
			}
		
			nMaxThundeCount = __min(nMaxThundeCount, (int)m_pCustomRule->cbBagCount - 2);
			nMaxThundeCount = __max(nMaxThundeCount, 2);
			for (int i = 1; i <= nMaxThundeCount; i++)
			{
				nThundeChance[i] = 100 / i;
			}
		}
		else if (!pIBankerUserItem->IsAndroidUser() && bIsUserWin)
		{
			nStartThundeIndex = 1;
			if (wReturnPer >= 50)
			{
				nMaxThundeCount = 4 * m_pCustomRule->cbBagCount / 10 + (wReturnPer - 50) / 10;
			}
			else
			{
				nMaxThundeCount = 4 * m_pCustomRule->cbBagCount / 10 - (50 - wReturnPer) / 10;
			}
			nMaxThundeCount = __min(nMaxThundeCount, (int)m_pCustomRule->cbBagCount - 2);
			nMaxThundeCount = __max(nMaxThundeCount, 2);
			for (int i = 1; i <= nMaxThundeCount; i++)
			{
				nThundeChance[i] = 100 / i;
			}
		}
		else if (m_lBagScore * m_pCustomRule->dBagTimes - m_lBagScore / (double)m_pCustomRule->cbBagCount > m_lBagScore)
		{
			nMaxThundeCount = 1;
		}	

		//��ҵ�ׯ���ϵͳ������
		if (!pIBankerUserItem->IsAndroidUser() && nMaxThundeCount > 1 && lUserStorage < m_lBagScore * m_pCustomRule->dBagTimes * (nMaxThundeCount - 1))
		{
			nMaxThundeCount = __max(1, lUserStorage / m_lBagScore * m_pCustomRule->dBagTimes);
		}


		int nRandAreaIndex = m_GameLogic.RandomArea(nThundeChance + nStartThundeIndex, nMaxThundeCount) + nStartThundeIndex;

		//��������
		int nRandCount = 0;
		while (cbThundePointCount != nThundeCount[nRandAreaIndex])
		{
			int nRandBagIndex = rand() % m_pCustomRule->cbBagCount;
			if (cbThundePointCount > nThundeCount[nRandAreaIndex])
			{
				for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
				{
					if (m_bBagThundePoint[i])
					{
						m_bBagThundePoint[i] = UpdateThunderInfo(false, m_dBagScore[i]);
						break;
					}
					nRandBagIndex = (nRandBagIndex + 1) % m_pCustomRule->cbBagCount;
				}
			}
			else
			{
				for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
				{
					if (!m_bBagThundePoint[i])
					{
						m_bBagThundePoint[i] = UpdateThunderInfo(true, m_dBagScore[i]);
						break;
					}
					nRandBagIndex = (nRandBagIndex + 1) % m_pCustomRule->cbBagCount;
				}
			}

			//ͳ������
			cbThundePointCount = 0;
			for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
			{
				m_bBagThundePoint[i] = m_GameLogic.IsHitThunderPoint(m_dBagScore[i], m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS);
				if (m_bBagThundePoint[i])
				{
					cbThundePointCount++;
				}
			}

			nRandCount++;
			if (nRandCount > 5000)
			{
				nRandCount = 0;
				nRandAreaIndex--;
			}
		}

		//������Ϣ
		while (cbThundePointCount > 0)
		{
			int nRandBagIndex = rand() % m_pCustomRule->cbBagCount;
			if (!m_bDebugThunderPoint[nRandBagIndex])
			{
				m_bDebugThunderPoint[nRandBagIndex] = true;
				cbThundePointCount--;
			}
		}

		//����
		for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
		{
			for (int j = i + 1; j < m_pCustomRule->cbBagCount; j++)
			{
				if (D_COMPARE(m_dBagScore[i], m_dBagScore[j]))
				{
					DOUBLE dTemp = m_dBagScore[i];
					m_dBagScore[i] = m_dBagScore[j];
					m_dBagScore[j] = dTemp;

					bool bTemp = m_bBagThundePoint[i];
					m_bBagThundePoint[i] = m_bBagThundePoint[j];
					m_bBagThundePoint[j] = bTemp;
				}
			}
		}
	}

	return true;
}

//���ɺ��
bool CTableFrameSink::GetBagDetail(IServerUserItem *pIServerUserItem, DOUBLE &dBagDetail)
{
	if (m_bDebugBag)
	{
		bool bGetBagAI = pIServerUserItem->IsAndroidUser();
		IServerUserItem *pIBankerUserItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
		bool bSetBagAI = pIBankerUserItem->IsAndroidUser();
		WORD wReturnPer = 100;
		LONGLONG lSystemStorage = 0, lUserStorage = 0;

		//ׯ��ΪAI
		if (bSetBagAI)
		{
			//�����Ƴ�AI
			if (!bGetBagAI)
			{
				wReturnPer = GetReturnPer(pIServerUserItem, lSystemStorage, lUserStorage);
			}
			else
			{
				wReturnPer = GetReturnPer(pIBankerUserItem, lSystemStorage, lUserStorage);
			}
		}
		else
		{
			//��������
			EM_CTRL_DEBUG userCtrlType = GetUserCtrlType(pIBankerUserItem);
			if (userCtrlType == USER_DEBUG)
			{
				wReturnPer = GetReturnPer(pIBankerUserItem, lSystemStorage, lUserStorage);
			}
			else
			{
				wReturnPer = GetReturnPer(pIServerUserItem, lSystemStorage, lUserStorage);
			}
		}

		int nBagIndex = rand() % m_pCustomRule->cbBagCount;

		//���⴦��
		bool bRealThunderPoint = true;
		if (m_wRealPlayerCount == 1)
		{
			if (m_nLastThundeCount > 2 || m_nNoThundeCount > 0)
			{
				if (!bGetBagAI)
				{
					if (m_nLastThundeCount > 2)
					{
						m_nNoThundeCount = 2 + rand() % 3;
						nBagIndex = m_pCustomRule->cbBagCount / 2 + (rand() % __max(1, m_pCustomRule->cbBagCount / 2));
					}
					else
					{
						nBagIndex = rand() % __max(1, m_pCustomRule->cbBagCount / 2);
					}
					bRealThunderPoint = false;					
				}
				else if (!bSetBagAI)
				{
					//һ���˵�ׯʱ
				}
			}
		}
		else
		{
			//�н��ʵ�ʱ����С���
			if (!bGetBagAI && bSetBagAI && wReturnPer <= 30)
			{
				nBagIndex = rand() % __max(1, m_pCustomRule->cbBagCount / 2);
			}
		}

		for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
		{
			nBagIndex = (nBagIndex + 1) % m_pCustomRule->cbBagCount;
			if (m_bDebugThunderPoint[m_cbAllGetCount] == m_bBagThundePoint[nBagIndex] && !m_bBagScore[nBagIndex])
			{
				if (m_bBagThundePoint[nBagIndex] && bRealThunderPoint || !m_bBagThundePoint[nBagIndex])
				{
					m_bBagScore[nBagIndex] = true;
					dBagDetail = m_dBagScore[nBagIndex];
					return m_bBagThundePoint[nBagIndex];
				}
			}
		}
		
		for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
		{
			nBagIndex = (nBagIndex + 1) % m_pCustomRule->cbBagCount;
			if (!m_bBagScore[nBagIndex])
			{
				m_bBagScore[nBagIndex] = true;
				dBagDetail = m_dBagScore[nBagIndex];
				return m_bBagThundePoint[nBagIndex];
			}
		}
		ASSERT(NULL);
		dBagDetail = 0;
		return false;
	}
	else
	{
		m_bBagScore[m_cbAllGetCount] = true;
		dBagDetail = m_dBagScore[m_cbAllGetCount];
		return m_bBagThundePoint[m_cbAllGetCount];
	}
}

//���ɺ��
bool CTableFrameSink::GetBagDetail(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE &dBagDetail)
{
	bool bThunderPoint = false;
	bool bIsUserWin = false;
	dBagDetail = m_GameLogic.RandDispatchMoney(dDispatchMoneySum, lDispatchCount);
	DOUBLE dBagBalance = dDispatchMoneySum - dBagDetail - MIN_POINT_TWO * (lDispatchCount - 1);
	bIsUserWin = (rand() % __max(100, (int)(50 * (m_pCustomRule->dBagTimes * m_pCustomRule->cbBagCount - 1)))) >= 50;
	bThunderPoint = UpdateThunderInfo(!bIsUserWin, dBagBalance, dBagDetail);
	ASSERT(bThunderPoint == m_GameLogic.IsHitThunderPoint(dBagDetail, m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS));

	return bThunderPoint;
}

//����÷�
DOUBLE CTableFrameSink::GetBagUserScore(WORD wChairID, DOUBLE dBagDetail, bool bThunderPoint)
{
	//��ҵ÷�
	SCORE lUserScore = (SCORE)(dBagDetail * GAME_TIME + 0.5);
	if (bThunderPoint)
		lUserScore -= (SCORE)(m_lBagScore*m_pCustomRule->dBagTimes*GAME_TIME);

	if (lUserScore > 0 && wChairID != m_wBankerUser)
	{
		SCORE lRevenue = m_pITableFrame->CalculateRevenue(wChairID, lUserScore);
		if (lRevenue > 0)
		{
			lUserScore -= lRevenue;
		}
	}
	return lUserScore / (double)GAME_TIME;
}

//�޸����׷�
bool CTableFrameSink::UpdateThunderInfo(bool bThunderPoint, DOUBLE &dBagDetail)
{
	if (bThunderPoint == m_GameLogic.IsHitThunderPoint(dBagDetail, m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS))
	{
		return bThunderPoint;
	}

	//��С����
	DOUBLE dTimes = m_pCustomRule->cbThunderPosition == 0 ? MIN_POINT_ONE : MIN_POINT_TWO;
	DOUBLE dBagBalance = 0.0;
	DOUBLE dBagChangeScore = 0.0;
	for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
	{
		if (m_bBagThundePoint[i])
		{
			dBagBalance += m_dBagScore[i] - dTimes * m_cbThunderPoint;
		}
		else
		{
			dBagBalance += m_dBagScore[i] - MIN_POINT_TWO;
		}
	}

	bThunderPoint = !bThunderPoint;
	if (bThunderPoint)
	{		
		if (D_COMPARE(dBagDetail, dTimes))
		{			
			dBagDetail -= dTimes;
			dBagBalance += dTimes;
			dBagChangeScore += dTimes;
		}
		else if (D_COMPARE(dBagBalance, dTimes))
		{
			dBagDetail += dTimes;
			dBagBalance -= dTimes;
			dBagChangeScore -= dTimes;
		}
	}
	else
	{
		//�׺�λ����
		BYTE cbBagTailNumber = (BYTE)m_GameLogic.GetNumPos(dBagDetail, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS);

		//��С����
		DOUBLE dTimes = m_pCustomRule->cbThunderPosition == 0 ? MIN_POINT_ONE : MIN_POINT_TWO;

		if (D_COMPARE(dBagDetail, ((10 - (m_cbThunderPoint - cbBagTailNumber)) % 10) * dTimes))
		{
			dBagDetail -= ((10 - (m_cbThunderPoint - cbBagTailNumber)) % 10) * dTimes;
			dBagBalance += ((10 - (m_cbThunderPoint - cbBagTailNumber)) % 10) * dTimes;
			dBagChangeScore += ((10 - (m_cbThunderPoint - cbBagTailNumber)) % 10) * dTimes;
		}
		else if (D_COMPARE(dBagBalance, ((10 - (cbBagTailNumber - m_cbThunderPoint)) % 10) * dTimes))
		{
			dBagDetail += ((10 - (cbBagTailNumber - m_cbThunderPoint)) % 10) * dTimes;
			dBagBalance -= ((10 - (cbBagTailNumber - m_cbThunderPoint)) % 10) * dTimes;
			dBagChangeScore -= ((10 - (cbBagTailNumber - m_cbThunderPoint)) % 10) * dTimes;
		}
	}

	//�޸��˺��
	if (!D_EPUALS(dBagChangeScore, 0.0))
	{
		bool bOk = false;
		int nBagIndex = rand() % m_pCustomRule->cbBagCount;
		for (int i = 0; i < m_pCustomRule->cbBagCount; i++)
		{
			nBagIndex = (nBagIndex + 1) % m_pCustomRule->cbBagCount;
			if (D_COMPARE(m_dBagScore[nBagIndex] + dBagChangeScore, MIN_POINT_TWO))
			{
				if (m_GameLogic.IsHitThunderPoint(m_dBagScore[nBagIndex], m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS) == 
					m_GameLogic.IsHitThunderPoint(m_dBagScore[nBagIndex] + dBagChangeScore, m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS))
				{
					m_dBagScore[nBagIndex] += dBagChangeScore;
					bOk = true;
					break;
				}
			}
		}

		//û�ҵ����������ĵ���
		if (!bOk)
		{
			while (D_EPUALS(dBagChangeScore, 0))
			{
				nBagIndex = (nBagIndex + 1) % m_pCustomRule->cbBagCount;

				if (D_COMPARE(dBagChangeScore, 0))
				{
					if (m_GameLogic.IsHitThunderPoint(m_dBagScore[nBagIndex], m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS) ==
						m_GameLogic.IsHitThunderPoint(m_dBagScore[nBagIndex] + MIN_POINT_TWO, m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS))
					{
						m_dBagScore[nBagIndex] += MIN_POINT_TWO;
						dBagChangeScore -= MIN_POINT_TWO;
					}
				}
				else
				{
					if (m_GameLogic.IsHitThunderPoint(m_dBagScore[nBagIndex] - MIN_POINT_TWO, m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS) ==
						m_GameLogic.IsHitThunderPoint(m_dBagScore[nBagIndex] - MIN_POINT_TWO, m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS))
					{
						m_dBagScore[nBagIndex] -= MIN_POINT_TWO;
						dBagChangeScore += MIN_POINT_TWO;
					}
				}
			}
		}
	}

	return !bThunderPoint;
}

//�޸����׷�
bool CTableFrameSink::UpdateThunderInfo(bool bThunderPoint, DOUBLE dBagBalance, DOUBLE &dBagDetail)
{
	if (bThunderPoint == m_GameLogic.IsHitThunderPoint(dBagDetail, m_cbThunderPoint, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS))
	{
		return bThunderPoint;
	}

	bThunderPoint = !bThunderPoint;
	if (bThunderPoint)
	{
		//��С����
		DOUBLE dTimes = m_pCustomRule->cbThunderPosition == 0 ? MIN_POINT_ONE : MIN_POINT_TWO;
		if (D_COMPARE(dBagDetail, dTimes))
		{
			dBagDetail -= dTimes;
			dBagBalance += dTimes;
		}
		else if (D_COMPARE(dBagBalance, dTimes))
		{
			dBagDetail += dTimes;
			dBagBalance -= dTimes;
		}
	}
	else
	{
		//�׺�λ����
		BYTE cbBagTailNumber = (BYTE)m_GameLogic.GetNumPos(dBagDetail, m_pCustomRule->cbThunderPosition ? DECIMALSECOND_POS : DECIMALFIRST_POS);

		//��С����
		DOUBLE dTimes = m_pCustomRule->cbThunderPosition == 0 ? MIN_POINT_ONE : MIN_POINT_TWO;

		if (D_COMPARE(dBagDetail, ((10 - (m_cbThunderPoint - cbBagTailNumber)) % 10) * dTimes))
		{
			dBagDetail -= ((10 - (m_cbThunderPoint - cbBagTailNumber)) % 10) * dTimes;
			dBagBalance += ((10 - (m_cbThunderPoint - cbBagTailNumber)) % 10) * dTimes;
		}
		else if (D_COMPARE(dBagBalance, ((10 - (cbBagTailNumber - m_cbThunderPoint)) % 10) * dTimes))
		{
			dBagDetail += ((10 - (cbBagTailNumber - m_cbThunderPoint)) % 10) * dTimes;
			dBagBalance -= ((10 - (cbBagTailNumber - m_cbThunderPoint)) % 10) * dTimes;
		}
	}

	return !bThunderPoint;
}

//������ʾ
void CTableFrameSink::SendDebugText(IServerUserItem * pISendUserItem, TCHAR * pText, ...)
{
	CMD_S_DebugText DebugText;

	// ת����Ϣ
	va_list VaList;
	va_start(VaList, pText);
	_vstprintf_s(DebugText.szMessageText, pText, VaList);
	va_end(VaList);

	// ������Ϣ
	m_pITableFrame->SendRoomData(pISendUserItem, SUB_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));
}

//���·����û���Ϣ
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, EM_USER_ACTION emUserAction, LONGLONG lGameScore)
{
	if (pIServerUserItem->IsAndroidUser()) { return; }

	//��������
	tagRoomUserInfo RoomUserInfo;
	ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));

	RoomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(RoomUserInfo.szNickName, pIServerUserItem->GetNickName(), sizeof(RoomUserInfo.szNickName));
	RoomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	RoomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();
	RoomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//�û����º�����
	if (emUserAction == USER_SITDOWN || emUserAction == USER_RECONNECT)
	{
		RoomUserInfo.wTableID = pIServerUserItem->GetTableID();
	}
	else if (emUserAction == USER_STANDUP || emUserAction == USER_OFFLINE)
	{
		RoomUserInfo.wTableID = INVALID_TABLE;
	}

	//�����Ѵ��ڵ��û�
	tagRoomUserInfo LastRoomUserInfo;
	ZeroMemory(&LastRoomUserInfo, sizeof(LastRoomUserInfo));
	if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), LastRoomUserInfo))
	{
		RoomUserInfo.lGameScore = LastRoomUserInfo.lGameScore + lGameScore;

		RoomUserInfo.bDebugIndividual = LastRoomUserInfo.bDebugIndividual;
		RoomUserInfo.tDebugTime = LastRoomUserInfo.tDebugTime;
		RoomUserInfo.tUpdateTime = LastRoomUserInfo.tUpdateTime;
		RoomUserInfo.dwDebugIndex = LastRoomUserInfo.dwDebugIndex;
		RoomUserInfo.dwGameID = LastRoomUserInfo.dwGameID;
		RoomUserInfo.lInitSystemStorage = LastRoomUserInfo.lInitSystemStorage;
		RoomUserInfo.lSystemStorage = LastRoomUserInfo.lSystemStorage;
		RoomUserInfo.lInitUserStorage = LastRoomUserInfo.lInitUserStorage;
		RoomUserInfo.lUserStorage = LastRoomUserInfo.lUserStorage;
		RoomUserInfo.nParameterK = LastRoomUserInfo.nParameterK;
	}

	m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), RoomUserInfo);
}

//��ȡ��ҵ�������
EM_CTRL_DEBUG CTableFrameSink::GetUserCtrlType(IServerUserItem *pIServerUserItem)
{
	//�����ȼ�������>����>ϵͳ���ԣ�
	//Ĭ��ϵͳ����
	//���Ҹ��˵���
	tagRoomUserInfo RoomUserInfo;
	ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
	if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
	{
		if (RoomUserInfo.bDebugIndividual)
		{
			return USER_DEBUG;
		}
	}

	//���ҷ������
	tagRoomDebugInfo RoomDebugInfo;
	ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
	if (GetFirstRoomDebugInfo(RoomDebugInfo))
	{
		return ROOM_DEBUG;
	}

	return SYS_DEBUG;
}

//���µ��Խ��
void CTableFrameSink::UpdateCtrlRes(IServerUserItem *pIServerUserItem, SCORE lTotalGameScore)
{
	//����0
	if (lTotalGameScore == 0)
	{
		return;
	}

	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN, lTotalGameScore);

	//���
	m_lStorageCurrent -= lTotalGameScore;

	//��������
	EM_CTRL_DEBUG emCtrlType = GetUserCtrlType(pIServerUserItem);

	SCORE lResetMinScore = (SCORE)(m_lBagScore*m_pCustomRule->dBagTimes*m_pCustomRule->nScoreRatio);
	switch (emCtrlType)
	{
	case USER_DEBUG:
	{
		//���Ҹ��˵���
		tagRoomUserInfo RoomUserInfo;
		ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
		if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			//����ʱ��
			RoomUserInfo.tUpdateTime = CTime::GetCurrentTime();

			//���Ӯ
			if (lTotalGameScore > 0)
			{
				RoomUserInfo.lUserStorage -= lTotalGameScore;
//  				if (RoomUserInfo.lUserStorage < 0)
//  				{
//  					RoomUserInfo.lSystemStorage -= RoomUserInfo.lUserStorage;
//  					RoomUserInfo.lUserStorage = 0;
//  				}

				//�������
				if (RoomUserInfo.lSystemStorage <= 0 && (RoomUserInfo.lUserStorage <= lResetMinScore || RoomUserInfo.bSystemStorageEnd))
				{
					RoomUserInfo.bDebugIndividual = false;

					//�����¼
					tagHistoryRoomUserInfo HistoryRoomUserInfo;
					HistoryRoomUserInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
					HistoryRoomUserInfo.dwGameID = pIServerUserItem->GetGameID();
					HistoryRoomUserInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
					HistoryRoomUserInfo.lDebugTime = RoomUserInfo.tDebugTime.GetTime();
					HistoryRoomUserInfo.lUpdateTime = RoomUserInfo.tUpdateTime.GetTime();
					HistoryRoomUserInfo.lInitSystemStorage = RoomUserInfo.lInitSystemStorage;
					HistoryRoomUserInfo.lSystemStorage = RoomUserInfo.lSystemStorage;
					HistoryRoomUserInfo.lInitUserStorage = RoomUserInfo.lInitUserStorage;
					HistoryRoomUserInfo.lUserStorage =RoomUserInfo.lUserStorage;
					HistoryRoomUserInfo.nParameterK =RoomUserInfo.nParameterK;
					HistoryRoomUserInfo.nDebugStatus = FINISH_STATUS;
					m_listHistoryRoomUserInfo.AddTail(HistoryRoomUserInfo);
					if (m_listHistoryRoomUserInfo.GetCount() > HISTORY_DEBUG_COUNT)
					{
						m_listHistoryRoomUserInfo.RemoveHead();
					}

					//���Ϳ�����
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.lSystemStorage, RoomUserInfo.lUserStorage, FINISH_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}
				else
				{

					//���Ϳ�����
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.lSystemStorage, RoomUserInfo.lUserStorage, RUNNNING_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}

				//��������
				m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), RoomUserInfo);
			}
			//ϵͳӮ
			else
			{
				RoomUserInfo.lSystemStorage -= (-lTotalGameScore);
// 				if (RoomUserInfo.lSystemStorage < 0 && !RoomUserInfo.bSystemStorageEnd)
//  				{
//  					RoomUserInfo.lUserStorage -= RoomUserInfo.lSystemStorage;
//  					RoomUserInfo.lSystemStorage = 0;
//  				}

				//�������
				if (RoomUserInfo.lSystemStorage <= 0 && (RoomUserInfo.lUserStorage <= lResetMinScore || RoomUserInfo.bSystemStorageEnd))
				{
					RoomUserInfo.bDebugIndividual = false;

					//�����¼
					tagHistoryRoomUserInfo HistoryRoomUserInfo;
					HistoryRoomUserInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
					HistoryRoomUserInfo.dwGameID = pIServerUserItem->GetGameID();
					HistoryRoomUserInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
					HistoryRoomUserInfo.lDebugTime = RoomUserInfo.tDebugTime.GetTime();
					HistoryRoomUserInfo.lUpdateTime = RoomUserInfo.tUpdateTime.GetTime();
					HistoryRoomUserInfo.lInitSystemStorage = RoomUserInfo.lInitSystemStorage;
					HistoryRoomUserInfo.lSystemStorage = RoomUserInfo.lSystemStorage;
					HistoryRoomUserInfo.lInitUserStorage = RoomUserInfo.lInitUserStorage;
					HistoryRoomUserInfo.lUserStorage = RoomUserInfo.lUserStorage;
					HistoryRoomUserInfo.nParameterK = RoomUserInfo.nParameterK;
					HistoryRoomUserInfo.nDebugStatus = FINISH_STATUS;
					m_listHistoryRoomUserInfo.AddTail(HistoryRoomUserInfo);
					if (m_listHistoryRoomUserInfo.GetCount() > HISTORY_DEBUG_COUNT)
					{
						m_listHistoryRoomUserInfo.RemoveHead();
					}

					//���Ϳ�����
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.lSystemStorage, RoomUserInfo.lUserStorage, FINISH_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}
				else
				{
					//���Ϳ�����
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.lSystemStorage, RoomUserInfo.lUserStorage, RUNNNING_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}

				//��������
				m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), RoomUserInfo);
			}
		}

		break;
	}
	case ROOM_DEBUG:
	{
		//���ҷ������
		tagRoomDebugInfo RoomDebugInfo;
		ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
		if (GetFirstRoomDebugInfo(RoomDebugInfo))
		{
			if (!(RoomDebugInfo.lSystemStorage <= 0 && RoomDebugInfo.lUserStorage <= 0))
			{
				//����ʱ��
				RoomDebugInfo.tUpdateTime = CTime::GetCurrentTime();

				//���Ӯ
				if (lTotalGameScore > 0)
				{
					RoomDebugInfo.lUserStorage -= lTotalGameScore;
//  					if (RoomDebugInfo.lUserStorage < 0)
//  					{
//  						RoomDebugInfo.lSystemStorage -= RoomDebugInfo.lUserStorage;
//  						RoomDebugInfo.lUserStorage = 0;
//  					}

					//�������
					if (RoomDebugInfo.lSystemStorage <= 0 && (RoomDebugInfo.lUserStorage <= lResetMinScore || RoomDebugInfo.bSystemStorageEnd))
					{
						//�����¼
						tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
						HistoryRoomDebugInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
						HistoryRoomDebugInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
						HistoryRoomDebugInfo.lDebugTime = RoomDebugInfo.tDebugTime.GetTime();
						HistoryRoomDebugInfo.lInitSystemStorage = RoomDebugInfo.lInitSystemStorage;
						HistoryRoomDebugInfo.lSystemStorage = RoomDebugInfo.lSystemStorage;
						HistoryRoomDebugInfo.lInitUserStorage = RoomDebugInfo.lInitUserStorage;
						HistoryRoomDebugInfo.lUserStorage = RoomDebugInfo.lUserStorage;
						HistoryRoomDebugInfo.nParameterK = RoomDebugInfo.nParameterK;
						HistoryRoomDebugInfo.nDebugStatus = FINISH_STATUS;
						m_listHistoryRoomDebugInfo.AddTail(HistoryRoomDebugInfo);
						if (m_listHistoryRoomDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
						{
							m_listHistoryRoomDebugInfo.RemoveHead();
						}

						//���÷����û���Ϣ
						RoomDebugInfo.nDebugStatus = FINISH_STATUS;;
						SetRoomDebugInfo(RoomDebugInfo);

						//���Ϳ�����
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.lSystemStorage, RoomDebugInfo.lUserStorage, FINISH_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//���ɾ����ֵ
						DelFirstRoomDebugInfo(RoomDebugInfo.dwDebugIndex);
					}
					else
					{
						//���Ϳ�����
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.lSystemStorage, RoomDebugInfo.lUserStorage, RUNNNING_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//���÷����û���Ϣ
						SetRoomDebugInfo(RoomDebugInfo);
					}
				}
				//ϵͳӮ
				else
				{
					RoomDebugInfo.lSystemStorage -= (-lTotalGameScore);
// 					if (RoomDebugInfo.lSystemStorage < 0 && !RoomDebugInfo.bSystemStorageEnd)
//  					{
//  						RoomDebugInfo.lUserStorage -= RoomDebugInfo.lSystemStorage;
//  						RoomDebugInfo.lSystemStorage = 0;
//  					}

					//�������
					if (RoomDebugInfo.lSystemStorage <= 0 && (RoomDebugInfo.lUserStorage <= lResetMinScore || RoomDebugInfo.bSystemStorageEnd))
					{
						//�����¼
						tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
						HistoryRoomDebugInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
						HistoryRoomDebugInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
						HistoryRoomDebugInfo.lDebugTime = RoomDebugInfo.tDebugTime.GetTime();
						HistoryRoomDebugInfo.lInitSystemStorage = RoomDebugInfo.lInitSystemStorage;
						HistoryRoomDebugInfo.lSystemStorage = RoomDebugInfo.lSystemStorage;
						HistoryRoomDebugInfo.lInitUserStorage = RoomDebugInfo.lInitUserStorage;
						HistoryRoomDebugInfo.lUserStorage = RoomDebugInfo.lUserStorage;
						HistoryRoomDebugInfo.nParameterK = RoomDebugInfo.nParameterK;
						HistoryRoomDebugInfo.nDebugStatus = FINISH_STATUS;
						m_listHistoryRoomDebugInfo.AddTail(HistoryRoomDebugInfo);
						if (m_listHistoryRoomDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
						{
							m_listHistoryRoomDebugInfo.RemoveHead();
						}

						//���÷����û���Ϣ
						RoomDebugInfo.nDebugStatus = FINISH_STATUS;;
						SetRoomDebugInfo(RoomDebugInfo);

						//���Ϳ�����
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.lSystemStorage, RoomDebugInfo.lUserStorage, FINISH_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//���ɾ����ֵ
						DelFirstRoomDebugInfo(RoomDebugInfo.dwDebugIndex);
					}
					else
					{
						//���Ϳ�����
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.lSystemStorage, RoomDebugInfo.lUserStorage, RUNNNING_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//���÷����û���Ϣ
						SetRoomDebugInfo(RoomDebugInfo);
					}
				}
			}
		}

		break;
	}
	case SYS_DEBUG:
	{
		//���Ӯ
		if (lTotalGameScore > 0)
		{
			m_lUserStorage -= lTotalGameScore;
// 			if (m_lUserStorage < 0)
// 			{
// 				m_lSystemStorage -= m_lUserStorage;
// 				m_lUserStorage = 0;
// 			}
		}
		//ϵͳӮ
		else
		{
			m_lSystemStorage -= (-lTotalGameScore);
// 			if (m_lSystemStorage < 0 && !m_bSystemStorageEnd)
// 			{
// 				m_lUserStorage -= m_lSystemStorage;
// 				m_lSystemStorage = 0;
// 			}
		}

		//����ʱ��
		m_tUpdateSystemStorage = CTime::GetCurrentTime();

		//�������
		if (m_lSystemStorage <= 0 && (m_lUserStorage <= lResetMinScore || m_bSystemStorageEnd))
		{
			//���Ϳ�����
			SendSysDebugInfo(m_dwSysDebugIndex, m_lSystemStorage, m_lUserStorage, FINISH_STATUS, m_tUpdateSystemStorage.GetTime());

			//�����¼
			tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
			HistoryRoomDebugInfo.dwDebugIndex = m_dwSysDebugIndex++;
			HistoryRoomDebugInfo.bSystemStorageEnd = m_bSystemStorageEnd;
			HistoryRoomDebugInfo.lDebugTime = m_tResetSystemStorage.GetTime();
			HistoryRoomDebugInfo.lInitSystemStorage = m_lLastInitSystemStorage;
			HistoryRoomDebugInfo.lSystemStorage = m_lSystemStorage;
			HistoryRoomDebugInfo.lInitUserStorage = m_lLastInitUserStorage;
			HistoryRoomDebugInfo.lUserStorage = m_lUserStorage;
			HistoryRoomDebugInfo.nParameterK = m_nParameterK;
			HistoryRoomDebugInfo.nDebugStatus = FINISH_STATUS;
			m_listHistorySysDebugInfo.AddTail(HistoryRoomDebugInfo);
			if (m_listHistorySysDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
			{
				m_listHistorySysDebugInfo.RemoveHead();
			}

			//����ֹ
			if (m_bSystemStorageEnd)
			{
				m_lSystemStorageEndScore += m_lUserStorage;
				//m_lSystemStorageEndScore -= m_lSystemStorage;
				m_lUserStorage = 0;
				//m_lSystemStorage = 0;
			}

			//�����ۼ�
			m_tResetSystemStorage = CTime::GetCurrentTime();
			m_tUpdateSystemStorage = m_tResetSystemStorage;
			m_nUpdateStorageCount++;

			m_lSystemStorage += m_lInitSystemStorage;
			m_lUserStorage += m_lInitUserStorage;
			m_nParameterK = m_nInitParameterK;

			//����ֹ���⴦��
			if (m_bSystemStorageEnd && (m_lUserStorage < 0 || m_lSystemStorage < 0))
			{
				m_nUpdateStorageCount++;
				m_dwSysDebugIndex++;

				m_lSystemStorage += m_lInitSystemStorage;
				m_lUserStorage += m_lInitUserStorage;
				m_nParameterK = m_nInitParameterK;

				if (m_lUserStorage < 0)
				{
					m_lUserStorage = 0;
				}

				if (m_lSystemStorage < 0)
				{
					m_lSystemStorage = 0;
				}

				if (m_lUserStorage <= 0 && m_lSystemStorage <= 0)
				{
					m_nUpdateStorageCount++;
					m_dwSysDebugIndex++;

					m_lSystemStorage = m_lInitSystemStorage;
					m_lUserStorage = m_lInitUserStorage;
					m_nParameterK = m_nInitParameterK;
				}
			}

			m_lLastInitSystemStorage = m_lSystemStorage;
			m_lLastInitUserStorage = m_lUserStorage;
		}
		else
		{
			//���Ϳ�����
			SendSysDebugInfo(m_dwSysDebugIndex, m_lSystemStorage, m_lUserStorage, RUNNNING_STATUS, m_tUpdateSystemStorage.GetTime());
		}
	}
	default:
		break;
	}
}


//ȡ���н�����
WORD CTableFrameSink::GetReturnPer(IServerUserItem *pIServerUserItem, LONGLONG &lSystemStorage, LONGLONG &lUserStorage)
{
	//��������
	EM_CTRL_DEBUG userCtrlType = GetUserCtrlType(pIServerUserItem);

	WORD wUserWinChance = INVALID_WORD;
	LONGLONG lDVal = 0;
	LONGLONG lMaxCtrlStorage = 0;
	int nParameterK = 0;
	bool bSysWin = false;

	switch (userCtrlType)
	{
	case USER_DEBUG:
	{
		//���Ҹ��˵���
		tagRoomUserInfo RoomUserInfo;
		ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
		if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			ASSERT(RoomUserInfo.bDebugIndividual);

			//���Բ�ֵ
			lDVal = abs(RoomUserInfo.lSystemStorage - RoomUserInfo.lUserStorage);
			lMaxCtrlStorage = max(RoomUserInfo.lSystemStorage, RoomUserInfo.lUserStorage);
			nParameterK = RoomUserInfo.nParameterK;
			bSysWin = RoomUserInfo.lSystemStorage > RoomUserInfo.lUserStorage;

			//��������ҿ����һ��Ϊ0ʱ
			if (RoomUserInfo.lUserStorage <= 0)
			{
				wUserWinChance = 0;
			}
			else if (RoomUserInfo.lSystemStorage <= 0)
			{
				wUserWinChance = 100;
			}

			lSystemStorage = RoomUserInfo.lSystemStorage;
			lUserStorage = RoomUserInfo.lUserStorage;
		}

		break;
	}
	case ROOM_DEBUG:
	{
		//���ҷ������
		tagRoomDebugInfo RoomDebugInfo;
		ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
		if (GetFirstRoomDebugInfo(RoomDebugInfo))
		{
			if (!(RoomDebugInfo.lSystemStorage <= 0 && RoomDebugInfo.lUserStorage <= 0))
			{
				//���Բ�ֵ
				lDVal = abs(RoomDebugInfo.lSystemStorage - RoomDebugInfo.lUserStorage);
				lMaxCtrlStorage = max(RoomDebugInfo.lSystemStorage, RoomDebugInfo.lUserStorage);
				nParameterK = RoomDebugInfo.nParameterK;
				bSysWin = RoomDebugInfo.lSystemStorage > RoomDebugInfo.lUserStorage;

				//��������ҿ����һ��Ϊ0ʱ
				if (RoomDebugInfo.lUserStorage <= 0)
				{
					wUserWinChance = 0;
				}
				else if (RoomDebugInfo.lSystemStorage <= 0)
				{
					wUserWinChance = 100;
				}

				lSystemStorage = RoomDebugInfo.lSystemStorage;
				lUserStorage = RoomDebugInfo.lUserStorage;
				break;
			}
		}

		break;
	}
	case SYS_DEBUG:
	{
		//���Բ�ֵ
		lDVal = abs(m_lSystemStorage - m_lUserStorage);
		lMaxCtrlStorage = max(m_lSystemStorage, m_lUserStorage);
		nParameterK = m_nParameterK;
		bSysWin = m_lSystemStorage > m_lUserStorage;

		//��������ҿ����һ��Ϊ0ʱ
		if (m_lUserStorage <= 0)
		{
			wUserWinChance = 0;
		}
		else if (m_lSystemStorage <= 0)
		{
			wUserWinChance = 100;
		}
		lSystemStorage = m_lSystemStorage;
		lUserStorage = m_lUserStorage;
		break;
	}
	default:
		break;
	}

	if (wUserWinChance == INVALID_WORD)
	{
		//��������ж�
		LONGLONG lCurParameterKScore = lMaxCtrlStorage * nParameterK / 100;
		if (lDVal <= lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 50 : 50;
		}
		else if (lDVal > lCurParameterKScore && lDVal <= 1.5 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 40 : 60;
		}
		else if (lDVal > 1.5 * lCurParameterKScore && lDVal <= 2 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 30 : 70;
		}
		else if (lDVal > 2 * lCurParameterKScore && lDVal <= 3 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 20 : 80;
		}
		else if (lDVal > 3 * lCurParameterKScore && lDVal <= 4 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 10 : 90;
		}
		else
		{
			wUserWinChance = bSysWin ? 0 : 100;
		}
	}

	return wUserWinChance;
}

//ȡ�÷����û���Ϣ
bool CTableFrameSink::GetRoomDebugInfo(DWORD dwDebugIndex, tagRoomDebugInfo &RoomDebugInfo)
{
	POSITION posHead = m_listRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		tagRoomDebugInfo RoomDebugInfoTemp = m_listRoomDebugInfo.GetNext(posHead);
		if (dwDebugIndex == RoomDebugInfoTemp.dwDebugIndex)
		{
			RoomDebugInfo = RoomDebugInfoTemp;
			return true;
		}
	}

	return false;
}

//���÷����û���Ϣ
bool CTableFrameSink::SetRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo)
{
	POSITION posHead = m_listRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		POSITION posCur = posHead;
		tagRoomDebugInfo RoomDebugInfoTemp = m_listRoomDebugInfo.GetNext(posHead);
		if (RoomDebugInfo.dwDebugIndex == RoomDebugInfoTemp.dwDebugIndex)
		{
			m_listRoomDebugInfo.SetAt(posCur, RoomDebugInfo);
			return true;
		}
	}

	return false;
}

//ȡ�÷����û���Ϣ
bool CTableFrameSink::GetFirstRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo)
{
	POSITION posHead = m_listRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		RoomDebugInfo = m_listRoomDebugInfo.GetNext(posHead);
		return true;
	}

	return false;
}

//ɾ�������û���Ϣ
bool CTableFrameSink::DelFirstRoomDebugInfo(DWORD dwDebugIndex)
{
	POSITION posHead = m_listRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		POSITION posCur = posHead;
		tagRoomDebugInfo RoomDebugInfo = m_listRoomDebugInfo.GetNext(posHead);
		if (dwDebugIndex == RoomDebugInfo.dwDebugIndex)
		{
			if (RoomDebugInfo.nDebugStatus != FINISH_STATUS)
			{
				//ִ�й��ı����¼
				if (RoomDebugInfo.nDebugStatus == RUNNNING_STATUS &&
					(RoomDebugInfo.lInitSystemStorage != RoomDebugInfo.lSystemStorage ||
					RoomDebugInfo.lInitUserStorage != RoomDebugInfo.lUserStorage))
				{
					//�����¼
					tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
					HistoryRoomDebugInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
					HistoryRoomDebugInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
					HistoryRoomDebugInfo.lDebugTime = RoomDebugInfo.tDebugTime.GetTime();
					HistoryRoomDebugInfo.lUpdateTime = CTime::GetCurrentTime().GetTime();
					HistoryRoomDebugInfo.lInitSystemStorage = RoomDebugInfo.lInitSystemStorage;
					HistoryRoomDebugInfo.lSystemStorage = RoomDebugInfo.lSystemStorage;
					HistoryRoomDebugInfo.lInitUserStorage = RoomDebugInfo.lInitUserStorage;
					HistoryRoomDebugInfo.lUserStorage = RoomDebugInfo.lUserStorage;
					HistoryRoomDebugInfo.nParameterK = RoomDebugInfo.nParameterK;
					HistoryRoomDebugInfo.nDebugStatus = CANCEL_STATUS;
					m_listHistoryRoomDebugInfo.AddTail(HistoryRoomDebugInfo);
					if (m_listHistoryRoomDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
					{
						m_listHistoryRoomDebugInfo.RemoveHead();
					}

					//���ͷ������
					SendRoomDebugInfo(HistoryRoomDebugInfo.dwDebugIndex, HistoryRoomDebugInfo.lSystemStorage, HistoryRoomDebugInfo.lUserStorage, CANCEL_STATUS, HistoryRoomDebugInfo.lUpdateTime);
				}
				else
				{
					//�������
					CMD_S_DeleteStorageInfo DeleteStorageInfo;
					DeleteStorageInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;

					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUB_S_DEL_ROOM_STORAGE, &DeleteStorageInfo, sizeof(DeleteStorageInfo));
				}
			}
			m_listRoomDebugInfo.RemoveAt(posCur);

			//���ҷ������
			tagRoomDebugInfo RoomDebugInfo;
			ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
			if (GetFirstRoomDebugInfo(RoomDebugInfo))
			{
				RoomDebugInfo.nDebugStatus = RUNNNING_STATUS;
				//���÷����û���Ϣ
				SetRoomDebugInfo(RoomDebugInfo);

				//���ͷ������
				SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.lSystemStorage, RoomDebugInfo.lUserStorage, RUNNNING_STATUS, RoomDebugInfo.tDebugTime.GetTime());
			}
			return true;
		}
	}

	return false;
}

//���Ϳ����Ϣ
bool CTableFrameSink::SendBaseStorageInfo(IServerUserItem *pIServerUserItem)
{
	//��������
	CMD_S_BaseStorageInfo BaseStorageInfo;
	BaseStorageInfo.lStartDebugTime = m_tStartDebugTime.GetTime();
	BaseStorageInfo.lResetSystemStorage = m_tResetSystemStorage.GetTime();;
	BaseStorageInfo.dwSysDebugIndex = m_dwSysDebugIndex;
	BaseStorageInfo.bSystemStorageEnd = m_bSystemStorageEnd;
	BaseStorageInfo.lInitSystemStorage = m_lInitSystemStorage;
	BaseStorageInfo.lLastInitSystemStorage = m_lLastInitSystemStorage;
	BaseStorageInfo.lSystemStorage = m_lSystemStorage;
	BaseStorageInfo.lInitUserStorage = m_lInitUserStorage;
	BaseStorageInfo.lLastInitUserStorage = m_lLastInitUserStorage;
	BaseStorageInfo.lUserStorage = m_lUserStorage;
	BaseStorageInfo.nInitParameterK = m_nInitParameterK;

	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_STORAGE, &BaseStorageInfo, sizeof(BaseStorageInfo));

	return true;
}

//������ҿ���
bool CTableFrameSink::SendUserDebugInfo(IServerUserItem *pIServerUserItem)
{
	return true;

	//��ҿ����־
	tagHistoryRoomUserInfo HistoryRoomUserInfo;
	ZeroMemory(&HistoryRoomUserInfo, sizeof(HistoryRoomUserInfo));

	// �����û���Ϣ��Ϣ
	WORD wBufferSize = 0;
	BYTE cbBuffer[IPC_PACKAGE];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//��������ɻ�ȡ��
	POSITION posHead = m_listHistoryRoomUserInfo.GetHeadPosition();
	while (posHead)
	{
		// ��������
		if ((wBufferSize + sizeof(HistoryRoomUserInfo) + 1) > sizeof(cbBuffer))
		{
			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_HISTORY_USER_DEBUG, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}
		
		HistoryRoomUserInfo = m_listHistoryRoomUserInfo.GetNext(posHead);

		// ������Ϣ
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomUserInfo, sizeof(HistoryRoomUserInfo));
		wBufferSize += sizeof(HistoryRoomUserInfo);
	}

	//����δ��ɻ�����е�
	DWORD dwGameID = 0;
	tagRoomUserInfo RoomUserInfo;
	ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
	POSITION pos = m_mapRoomUserInfo.GetStartPosition();
	while (pos)
	{
		// ��������
		if ((wBufferSize + sizeof(HistoryRoomUserInfo) + 1) > sizeof(cbBuffer))
		{
			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_HISTORY_USER_DEBUG, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}

		m_mapRoomUserInfo.GetNextAssoc(pos, dwGameID, RoomUserInfo);

		//���
		if (RoomUserInfo.bDebugIndividual)
		{
			HistoryRoomUserInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
			HistoryRoomUserInfo.dwGameID = pIServerUserItem->GetGameID();
			HistoryRoomUserInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
			HistoryRoomUserInfo.lDebugTime = RoomUserInfo.tDebugTime.GetTime();
			HistoryRoomUserInfo.lUpdateTime = RoomUserInfo.tUpdateTime.GetTime();
			HistoryRoomUserInfo.lInitSystemStorage = RoomUserInfo.lInitSystemStorage;
			HistoryRoomUserInfo.lSystemStorage = RoomUserInfo.lSystemStorage;
			HistoryRoomUserInfo.lInitUserStorage = RoomUserInfo.lInitUserStorage;
			HistoryRoomUserInfo.lUserStorage = RoomUserInfo.lUserStorage;
			HistoryRoomUserInfo.nParameterK = RoomUserInfo.nParameterK;
			HistoryRoomUserInfo.nDebugStatus = RUNNNING_STATUS;

			// ������Ϣ
			CopyMemory(cbBuffer + wBufferSize, &HistoryRoomUserInfo, sizeof(HistoryRoomUserInfo));
			wBufferSize += sizeof(HistoryRoomUserInfo);
		}
	}

	// ��Ч��Ϣ
	if (wBufferSize > 0)
	{
		// ������Ϣ
		m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_HISTORY_USER_DEBUG, cbBuffer, wBufferSize);
	}

	return false;
}

//������ҿ���
bool CTableFrameSink::SendUserDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem)
{
	return true;

	//��ҿ����־
	CMD_S_UpdateStorageInfo UpdateStorageInfo;
	UpdateStorageInfo.dwDebugIndex = dwDebugIndex;
	UpdateStorageInfo.lSystemStorage = lSystemStorage;
	UpdateStorageInfo.lUserStorage = lUserStorage;
	UpdateStorageInfo.nDebugStatus = nDebugStatus;
	UpdateStorageInfo.lUpdateTime = lUpdateTime;

	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_USER_DEBUG, &UpdateStorageInfo, sizeof(UpdateStorageInfo));

	return true;
}

//���ͷ������
bool CTableFrameSink::SendRoomDebugInfo(IServerUserItem *pIServerUserItem)
{	
	// �����û���Ϣ��Ϣ
	WORD wBufferSize = 0;
	BYTE cbBuffer[IPC_PACKAGE];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));
	tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
	ZeroMemory(&HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));

	//��������ɻ�ȡ��
	POSITION posHead = m_listHistoryRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		HistoryRoomDebugInfo = m_listHistoryRoomDebugInfo.GetNext(posHead);

		// ��������
		if ((wBufferSize + sizeof(HistoryRoomDebugInfo) + 1) > sizeof(cbBuffer))
		{
			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_HISTORY_ROOM_DEBUG, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}

		// ������Ϣ
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));
		wBufferSize += sizeof(HistoryRoomDebugInfo);
	}

	//����δ��ɻ�����е�
	tagRoomDebugInfo RoomDebugInfo;
	ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
	posHead = m_listRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		RoomDebugInfo = m_listRoomDebugInfo.GetNext(posHead);

		HistoryRoomDebugInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
		HistoryRoomDebugInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
		HistoryRoomDebugInfo.lDebugTime = RoomDebugInfo.tDebugTime.GetTime();
		HistoryRoomDebugInfo.lUpdateTime = RoomDebugInfo.tUpdateTime.GetTime();
		HistoryRoomDebugInfo.lInitSystemStorage = RoomDebugInfo.lInitSystemStorage;
		HistoryRoomDebugInfo.lSystemStorage = RoomDebugInfo.lSystemStorage;
		HistoryRoomDebugInfo.lInitUserStorage = RoomDebugInfo.lInitUserStorage;
		HistoryRoomDebugInfo.lUserStorage = RoomDebugInfo.lUserStorage;
		HistoryRoomDebugInfo.nParameterK = RoomDebugInfo.nParameterK;
		HistoryRoomDebugInfo.nDebugStatus = RoomDebugInfo.nDebugStatus;

		// ��������
		if ((wBufferSize + sizeof(HistoryRoomDebugInfo) + 1) > sizeof(cbBuffer))
		{
			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_HISTORY_ROOM_DEBUG, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}

		// ������Ϣ
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));
		wBufferSize += sizeof(HistoryRoomDebugInfo);
	}

	// ��Ч��Ϣ
	if (wBufferSize > 0)
	{
		// ������Ϣ
		m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_HISTORY_ROOM_DEBUG, cbBuffer, wBufferSize);
	}


	return true;
}

//���ͷ������
bool CTableFrameSink::SendRoomDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem)
{
	//��ҿ����־
	CMD_S_UpdateStorageInfo UpdateStorageInfo;
	UpdateStorageInfo.dwDebugIndex = dwDebugIndex;
	UpdateStorageInfo.lSystemStorage = lSystemStorage;
	UpdateStorageInfo.lUserStorage = lUserStorage;
	UpdateStorageInfo.nDebugStatus = nDebugStatus;
	UpdateStorageInfo.lUpdateTime = lUpdateTime;

	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_ROOM_DEBUG, &UpdateStorageInfo, sizeof(UpdateStorageInfo));

	return true;
}

//����ϵͳ����
bool CTableFrameSink::SendSysDebugInfo(IServerUserItem *pIServerUserItem)
{
	tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
	ZeroMemory(&HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));

	// �����û���Ϣ��Ϣ
	WORD wBufferSize = 0;
	BYTE cbBuffer[IPC_PACKAGE];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	POSITION posHead = m_listHistorySysDebugInfo.GetHeadPosition();
	while (posHead)
	{
		HistoryRoomDebugInfo = m_listHistorySysDebugInfo.GetNext(posHead);

		// ��������
		if ((wBufferSize + sizeof(HistoryRoomDebugInfo) + 1) > sizeof(cbBuffer))
		{
			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_HISTORY_SYS_DEBUG, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}

		// ������Ϣ
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));
		wBufferSize += sizeof(HistoryRoomDebugInfo);
	}

	// ��Ч��Ϣ
	if (wBufferSize > 0)
	{
		// ������Ϣ
		m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_HISTORY_SYS_DEBUG, cbBuffer, wBufferSize);
	}

	//���Ϳ�����
	SendSysDebugInfo(m_dwSysDebugIndex, m_lSystemStorage, m_lUserStorage, RUNNNING_STATUS, m_tUpdateSystemStorage.GetTime(), pIServerUserItem);


	return true;
}

//����ϵͳ����
bool CTableFrameSink::SendSysDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem)
{
	//��ҿ����־
	CMD_S_UpdateStorageInfo UpdateStorageInfo;
	UpdateStorageInfo.dwDebugIndex = dwDebugIndex;
	UpdateStorageInfo.lSystemStorage = lSystemStorage;
	UpdateStorageInfo.lUserStorage = lUserStorage;
	UpdateStorageInfo.nDebugStatus = nDebugStatus;
	UpdateStorageInfo.lUpdateTime = lUpdateTime;

	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_SYS_DEBUG, &UpdateStorageInfo, sizeof(UpdateStorageInfo));

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////