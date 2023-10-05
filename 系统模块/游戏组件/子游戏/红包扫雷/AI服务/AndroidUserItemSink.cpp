#include "Stdafx.h"
#include "AndroidUserItemSink.h"
//////////////////////////////////////////////////////////////////////////

//��Ϸʱ��
#define IDI_START_GAME				1									//���ƶ�ʱ��
#define IDI_USER_OPERATE			2									//��ʼ��ʱ��
#define IDI_USER_BANK				3									//��ʼ��ʱ��

#define TIME_USER_OPERATE			5									//����ʱ��

//////////////////////////////////////////////////////////////////////////
//������Ϣ
int				CAndroidUserItemSink::m_nScoreRatio = GAME_TIME;		//��������
BYTE			CAndroidUserItemSink::m_cbPlayTime = 8;					//����ʱ��
BYTE			CAndroidUserItemSink::m_cbBagCount = 10;				//�������
LONGLONG		CAndroidUserItemSink::m_lScoreRangeMin = 10;			//�����Сֵ
LONGLONG		CAndroidUserItemSink::m_lScoreRangeMax = 1000;			//������ֵ
LONGLONG		CAndroidUserItemSink::m_lDispatchCondition = 10;		//����Ҫ��
DOUBLE			CAndroidUserItemSink::m_dBagTimes = 1.0;				//����
BYTE			CAndroidUserItemSink::m_cbThunderPoint = 0;				//����׺�
BYTE			CAndroidUserItemSink::m_cbThunderPositon = 1;			//�׺�λ�ã�0С�����һλ��1С����ڶ�λ

BYTE			CAndroidUserItemSink::m_cbMinBagListCount = 3;			//�����б������������ֵʱ��ʼ���ף�
BYTE			CAndroidUserItemSink::m_cbMaxRealBagListCount = 6;		//�����б����������������������ֵʱ�����ף�
BYTE			CAndroidUserItemSink::m_cbSetBagCount[2] = {3, 5};		//���׸���
BYTE			CAndroidUserItemSink::m_cbSetBagTime[2] = {10, 20};		//���׼��
LONGLONG		CAndroidUserItemSink::m_lSetBagScore[2] = { 10, 200 };	//���׺����С
BYTE			CAndroidUserItemSink::m_cbStartGetBagTime = 2;			//�������ʼʱ��
BYTE			CAndroidUserItemSink::m_cbGetBagTime[2] = {1, 3};		//��������
BYTE			CAndroidUserItemSink::m_cbMaxGetBagCount = 3;			//������������
BYTE			CAndroidUserItemSink::m_cbLastNoGetBagCount = 3;		//������������
BYTE			CAndroidUserItemSink::m_cbRandSetBagCount = 4;			//���׸���
BYTE			CAndroidUserItemSink::m_cbRandSetBagTime = 15;			//���׼��
BYTE			CAndroidUserItemSink::m_cbRandGetBagTime = 3;			//��������
BYTE			CAndroidUserItemSink::m_cbAIGetBagCount = 0;			//AI���������
int				CAndroidUserItemSink::m_nRandSetBagMaxCount = 0;		//������������޼��ʱ��
DWORD			CAndroidUserItemSink::m_dwLastSetBagTime = 0;			//�������ʱ��
DWORD			CAndroidUserItemSink::m_dwLastSetBagCountTime = 0;		//������������޸�ʱ��
DWORD			CAndroidUserItemSink::m_dwLastGetBagTime = 0;			//��������ʱ��
//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
void *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);

	return NULL;
}

//��ʼ�ӿ�
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem)!=NULL);
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);

	//�����ж�
	if (m_pIAndroidUserItem==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//���ýӿ�
bool  CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//ʱ����Ϣ
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//��ʼ��ʱ��
		{
			//������Ϣ
			m_pIAndroidUserItem->SendUserReady(NULL, NULL);

			return true;
		}
	case IDI_USER_OPERATE:		//��ʼ��ʱ��
		{	
			//������ʱ��
			UINT  nElapse = 1 + (rand() % TIME_USER_OPERATE);
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_OPERATE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_OPERATE, nElapse);

			DWORD dwCurTime = GetTickCount();
			DWORD dwTimeSpan = dwCurTime - m_dwLastSetBagCountTime;

			//�������޼��
			if (dwTimeSpan > m_nRandSetBagMaxCount * 1000)
			{
				m_dwLastSetBagCountTime = dwCurTime;
				m_nRandSetBagMaxCount = (15 + (rand() % 15)) * 60;
				m_cbRandSetBagCount = m_cbSetBagCount[0] + (rand() % __max(1, m_cbSetBagCount[1] - m_cbSetBagCount[0]));
			}

			//�Ƿ�����
			if (IsGetBag())
			{
				//���������ҷ���
				if (m_pIAndroidUserItem->GetMeUserItem()->GetUserScore() - m_lSetBagTotalScore >= m_dBagTimes*m_lBagScore*m_nScoreRatio)
				{
					m_wGetUser[m_cbGetCount] = m_pIAndroidUserItem->GetChairID();
					m_cbAIGetBagCount++;
					m_cbGetCount++;

					//�����
					CMD_C_GetBag GetBag;
					GetBag.lBagKey = m_lBagKey;

					//����Ϣ
					m_pIAndroidUserItem->SendSocketData(SUB_C_GET_BAG, &GetBag, sizeof(GetBag));

					//����ʱ��
					m_dwLastGetBagTime = dwCurTime;
					m_cbRandGetBagTime = m_cbGetBagTime[0] + (rand() % __max(1, m_cbGetBagTime[1] - m_cbGetBagTime[0]));

					return true;
				}
			}

			//�Ƿ�����
			if (IsSetBag())
			{
				LONGLONG lMinBagScore = m_lSetBagScore[0];
				LONGLONG lMaxBagScore = m_lSetBagScore[1];
// 				if (m_wRealSetBagCount > 0)
// 				{
// 					lMinBagScore = m_lScoreRangeMin;
// 					lMaxBagScore = 0;
// 					int nBagCount = m_vecRedBag.size();
// 					for (int i = 0; i < nBagCount; i++)
// 					{
// 						IServerUserItem *pDispatchUserUserItem = m_pIAndroidUserItem->GetTableUserItem(m_vecRedBag[i].wDispatchUser);
// 						if (pDispatchUserUserItem != NULL)
// 						{
// 							if (!pDispatchUserUserItem->IsAndroidUser())
// 							{
// 								if (m_vecRedBag[i].lBagScore < lMaxBagScore || lMaxBagScore == 0)
// 								{
// 									lMaxBagScore = m_vecRedBag[i].lBagScore;
// 								}
// 							}
// 						}
// 						
// 					}
// 				}

				LONGLONG lScore = lMinBagScore + (rand() % __max(1, lMaxBagScore - lMinBagScore));

				//ȡ��
				if (rand() % 100 < 90)
				{
					lScore = lScore / 10 * 10;
				}

				//�жϷ���
				if (m_pIAndroidUserItem->GetMeUserItem()->GetUserScore() < m_nScoreRatio * lScore)
				{
					return true;
				}

				//�����
				CMD_C_DispatchBag DispatchBag;
				DispatchBag.lScore = lScore;
				DispatchBag.lThunderPoint = rand() % 10;

				//����Ϣ
				m_pIAndroidUserItem->SendSocketData(SUB_C_DISPATCH_BAG, &DispatchBag, sizeof(DispatchBag));

				//����ʱ��
				m_cbRandSetBagTime = m_cbSetBagTime[0] + (rand() % __max(1, m_cbSetBagTime[1] - m_cbSetBagTime[0]));
				m_dwLastSetBagTime = dwCurTime;

				return true;
			}

			return true;
		}
	case IDI_USER_BANK:		//��ȡ�ʱ��
		{			
			m_pIAndroidUserItem->KillGameTimer(nTimerID);

			//��������
			IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			LONGLONG lRobotScore = pUserItem->GetUserScore();	
			tagAndroidParameter *pAndroidParameter = m_pIAndroidUserItem->GetAndroidParameter();

			if (lRobotScore < pAndroidParameter->lTakeMinScore / 2)
			{
				SCORE lScore = pAndroidParameter->lTakeMinScore + (rand() % __max(pAndroidParameter->lTakeMaxScore - pAndroidParameter->lTakeMinScore, 1));
				if (lScore > 0)
				{
					m_pIAndroidUserItem->PerformTakeScore(lScore);
				}
			}
			else if (lRobotScore > pAndroidParameter->lTakeMaxScore * 2)
			{
				SCORE lScore = rand() % __max(lRobotScore - pAndroidParameter->lTakeMinScore, 1);
				if (lScore > 0)
				{
					m_pIAndroidUserItem->PerformSaveScore(lScore);
				}
			}

			return true;
		}	
	}
	return false;
}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_START:			//��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_DISPATCH_BAG:	//�ɷ����
		{
			return OnSubDispatchBag(pData,wDataSize);
		}
	case SUB_S_CANCEL_BAG:		//ȡ�����
		{
			return OnSubCancelBag(pData,wDataSize);
		}
	case SUB_S_GET_BAG:			//�����
		{
			return OnSubGetBag(pData,wDataSize);
		}
	case SUB_S_GET_BAG_FAIL:	//����ʧ��
		{
			return OnSubGetBagFail(pData, wDataSize);
		}
	case SUB_S_END:				//��������
		{
			return OnSubGameConclude(pData,wDataSize);
		}	
	}
	return true;

}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//����״̬
	case GAME_SCENE_PLAY:	//��Ϸ״̬
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_S_StatusPlayAI));
			if (wDataSize != sizeof(CMD_S_StatusPlayAI)) return false;

			//��������
			CMD_S_StatusPlayAI * pStatusPlayAI = (CMD_S_StatusPlayAI *)pData;

			//������Ϣ
			m_nScoreRatio = pStatusPlayAI->StatusPlay.nScoreRatio;
			m_cbLeftTime = pStatusPlayAI->StatusPlay.cbLeftTime;
			m_cbPlayTime = pStatusPlayAI->StatusPlay.cbPlayTime;
			m_cbBagCount = pStatusPlayAI->StatusPlay.cbBagCount;
			m_lScoreRangeMin = pStatusPlayAI->StatusPlay.lScoreRangeMin;
			m_lScoreRangeMax = pStatusPlayAI->StatusPlay.lScoreRangeMax;
			m_lDispatchCondition = pStatusPlayAI->StatusPlay.lDispatchCondition;
			m_dBagTimes = pStatusPlayAI->StatusPlay.dBagTimes;
			m_cbThunderPoint = pStatusPlayAI->StatusPlay.cbThunderPoint;
			m_cbThunderPositon = pStatusPlayAI->StatusPlay.cbThunderPositon;

			//AI����
			m_cbMinBagListCount = pStatusPlayAI->cbMinBagListCount;;
			m_cbMaxRealBagListCount = pStatusPlayAI->cbMaxRealBagListCount;
			CopyMemory(m_cbSetBagCount, pStatusPlayAI->cbSetBagCount, sizeof(m_cbSetBagCount));
			CopyMemory(m_cbSetBagTime, pStatusPlayAI->cbSetBagTime, sizeof(m_cbSetBagTime));
			CopyMemory(m_lSetBagScore, pStatusPlayAI->lSetBagScore, sizeof(m_lSetBagScore));
			m_cbStartGetBagTime = pStatusPlayAI->cbStartGetBagTime;
			CopyMemory(m_cbGetBagTime, pStatusPlayAI->cbGetBagTime, sizeof(m_cbGetBagTime));
			m_cbMaxGetBagCount = pStatusPlayAI->cbMaxGetBagCount;
			m_cbLastNoGetBagCount = pStatusPlayAI->cbLastNoGetBagCount;

			//��Ϸ����
			m_lSetBagTotalScore = 0;
			m_vecRedBag.clear();
			m_lBagKey = pStatusPlayAI->StatusPlay.lBagKey;
			m_wBankerUser = pStatusPlayAI->StatusPlay.wBankerUser;
			m_lBagScore = pStatusPlayAI->StatusPlay.lScore;
			m_cbGetCount = m_cbBagCount - pStatusPlayAI->StatusPlay.cbLeftCount;
			CopyMemory(m_wGetUser, pStatusPlayAI->StatusPlay.wGetUser, sizeof(m_wGetUser));


			//������ʱ��
			UINT  nElapse = 1 + (rand() % TIME_USER_OPERATE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_OPERATE, nElapse);

			return true;
		}


	}

	return true;
}

//�û�����
void  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
void  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
void  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
void  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//ɾ�����
	m_vecRedBag.erase(m_vecRedBag.begin());

	//��������
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//���ñ���
	m_wBankerUser = pGameStart->wBankerUser;
	m_lBagKey = pGameStart->lBagKey;
	m_lBagScore = pGameStart->lScore;
	m_cbThunderPoint = pGameStart->cbThunderPoint;
	m_dBagBalance = (double)m_lBagScore;
	m_cbGetCount = 0;
	m_cbAIGetBagCount = 0;

	//�Լ����ĺ��
	if (m_wBankerUser == m_pIAndroidUserItem->GetChairID())
	{
		m_lSetBagTotalScore -= m_lBagScore * m_nScoreRatio;
	}

	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_PLAY);
	m_dwStartGetBagTime = GetTickCount();

	return true;
}

//�ɷ����
bool CAndroidUserItemSink::OnSubDispatchBag(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_S_DispatchBag));
	if (wDataSize != sizeof(CMD_S_DispatchBag)) return false;

	//��������
	CMD_S_DispatchBag * pDispatchBag = (CMD_S_DispatchBag *)pData;
	

	//�����б�
	stRedBagInfo redBag;
	ZeroMemory(&redBag, sizeof(redBag));
	redBag.wDispatchUser = pDispatchBag->wDispatchUser;
	redBag.lBagScore = pDispatchBag->lScore;
	redBag.cbThunderPoint = 0;
	redBag.lBagKey = pDispatchBag->lBagKey;

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

	if (bInsert == false)
	{
		m_vecRedBag.push_back(redBag);
	}

	if (redBag.wDispatchUser == m_pIAndroidUserItem->GetChairID())
	{
		m_lSetBagTotalScore += pDispatchBag->lScore * m_nScoreRatio;
	}

	return true;
}

//ȡ�����
bool CAndroidUserItemSink::OnSubCancelBag(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_S_CancelBag));
	if (wDataSize != sizeof(CMD_S_CancelBag)) return false;

 	//��������
 	CMD_S_CancelBag * pCancelBag = (CMD_S_CancelBag *)pData;

	//ɾ�����
	for (UINT i = 0; i < m_vecRedBag.size();)
	{
		if (m_vecRedBag[i].lBagKey == pCancelBag->lBagKey)
		{
			//�Լ����ĺ��
			if (m_vecRedBag[i].wDispatchUser == m_pIAndroidUserItem->GetChairID())
			{
				m_lSetBagTotalScore -= m_lBagScore * m_nScoreRatio;
			}
			m_vecRedBag.erase(m_vecRedBag.begin() + i);
		}
		else
		{
			i++;
		}
	}

	return true;
}

//�����
bool CAndroidUserItemSink::OnSubGetBag(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_S_GetBag));
	if (wDataSize != sizeof(CMD_S_GetBag)) return false;

	//��������
	CMD_S_GetBag * pGetBag = (CMD_S_GetBag *)pData;
	
	//���ñ���
	BYTE cbGetCount = m_cbBagCount - pGetBag->cbLeftCount - 1;
	m_wGetUser[cbGetCount] = pGetBag->wGetUser;
	m_dBagDetail[cbGetCount] = pGetBag->dBagScore;
	m_bThunderPoint[cbGetCount] = pGetBag->bThunderPoint;
	m_dBagBalance -= m_dBagDetail[cbGetCount];

	m_cbGetCount++;
	if (pGetBag->wGetUser == m_pIAndroidUserItem->GetChairID())
	{
		m_cbAIGetBagCount--;
	}

	return true;
}

//����ʧ��
bool CAndroidUserItemSink::OnSubGetBagFail(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_S_GetBag_Fail));
	if (wDataSize != sizeof(CMD_S_GetBag_Fail)) return false;

	//��������
	CMD_S_GetBag_Fail * pGetBag_Fail = (CMD_S_GetBag_Fail *)pData;

	if (pGetBag_Fail->wGetUser == m_pIAndroidUserItem->GetChairID())
	{
		m_cbGetCount--;
		m_cbAIGetBagCount--;
	}

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameConclude(VOID * pData, WORD wDataSize)
{
	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_FREE);
	
	m_wBankerUser = INVALID_CHAIR;
	m_lBagKey = 0;

	return true;
}

//�Ƿ�����
bool CAndroidUserItemSink::IsSetBag()
{
	int nBagListCount = m_vecRedBag.size();

	//����Ϸ�������б��е���������һ��������ʱ��AI����
	if (m_cbMinBagListCount > nBagListCount)
	{
		int nRealSetBagCount = 0;
		for (UINT i = 0; i < m_vecRedBag.size(); i++)
		{
			IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(m_vecRedBag[i].wDispatchUser);
			if (pIServerUserItem != NULL)
			{
				if (!pIServerUserItem->IsAndroidUser())
				{
					nRealSetBagCount++;
				}
			}
		}

		//AI���׺������б�����ʵ��ҵ���������һ��������ʱ��AIȡ������
		if (nRealSetBagCount <= m_cbMaxRealBagListCount)
		{
			//��������
			if (nBagListCount - nRealSetBagCount < m_cbRandSetBagCount)
			{
				DWORD dwCurTime = GetTickCount();
				DWORD dwTimeSpan = dwCurTime - m_dwLastSetBagTime;
				//���׼��
				if (dwTimeSpan > m_cbRandSetBagTime * 1000)
				{
					return true;
				}
			}
		}
	}
	return false;
}

//�Ƿ�����
bool CAndroidUserItemSink::IsGetBag()
{
	//�޺������
	if (m_wBankerUser == INVALID_CHAIR || m_lBagKey == 0)
	{
		return false;
	}

	DWORD dwCurTime = GetTickCount();
	DWORD dwTimeSpan = dwCurTime - m_dwStartGetBagTime;

	//ǰ���벻��,��ʱ����
	if (dwTimeSpan < m_cbStartGetBagTime * 1000 || dwTimeSpan >= m_cbPlayTime * 1000)
	{
		return false;
	}

	//���̫�̲���
	dwTimeSpan = dwCurTime - m_dwLastGetBagTime;
	if (dwTimeSpan < m_cbRandGetBagTime * 1000)
	{
		return false;
	}

	//����������޲���
	if (m_cbGetCount + m_cbAIGetBagCount >= m_cbMaxGetBagCount)
	{
		return false;
	}

	//�����ٸ��������
	if (m_cbLastNoGetBagCount >= m_cbBagCount - m_cbGetCount - m_cbAIGetBagCount)
	{
		return false;
	}

	//�����ϴεĺ������
	for (int i = 0; i < m_cbGetCount; i++)
	{		
		if (m_wGetUser[i] == m_pIAndroidUserItem->GetChairID())
		{
			return false;
		}
	}

	return true;
}

//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
