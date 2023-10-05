#include "StdAfx.h"
#include "AndroidUserItem.h"

//////////////////////////////////////////////////////////////////////////////////

//��̬����
CTimerItemArray CAndroidUserItem::m_TimerItemBuffer;								//�������

#define IDI_ANDROID_LOGONHANGING			1000									//��¼�һ���ʱ��
#define IDI_ANDROID_CHECKHANGING			1001									//�л��һ���ʱ��

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItem::CAndroidUserItem()
{
	//��������
	m_wRoundID = 1;
	m_wAndroidIndex = INVALID_WORD;

	//�ӿڱ���
	m_pIServerUserManager = NULL;
	m_pIAndroidUserManager = NULL;
	m_pIAndroidUserItemSink = NULL;

	//״̬����
	m_bWaitLeave = false;
	m_bStartClient = false;
	m_wAndroidAction = 0;
	m_wAndroidGrade = AI_CARRY_GRADE_NONE | AI_LEAVE_GRADE_NONE;
	m_wAndroidChatType = ANDROID_INVALID_CT;
	m_cbGameStatus = GAME_STATUS_FREE;

	//�û�״̬
	m_CurrentUserStatus.cbUserStatus = US_NULL;
	m_CurrentUserStatus.wChairID = INVALID_CHAIR;
	m_CurrentUserStatus.wTableID = INVALID_TABLE;

	m_bHanging = false;
	m_bInitialIn = false;
	m_wSwitchTableRate = 0;
	m_wFreeSameTableSec = 0;
	m_TableItemActive.RemoveAll();

	//�󶨱���
	m_pIServerUserItem = NULL;

	//ʱ�����
	m_dwMinSitInterval = 0;
	m_dwMaxSitInterval = 0;
	m_dwStandupTickCount = 0;
	m_dwReposeTickCount = 0;
	m_dwLastChatMessageTickCount = 0;
	m_dwLogonFinishTickCount = 0;

	//״̬��Ϣ
	ZeroMemory(&m_AndroidAccountsInfo, sizeof(m_AndroidAccountsInfo));

	return;
}

//��������
CAndroidUserItem::~CAndroidUserItem()
{
	//ɾ��ʱ��
	KillGameTimer(0);

	//�ͷŶ���
	SafeRelease(m_pIAndroidUserItemSink);

	//ɾ���洢
	for (INT_PTR i = 0; i<m_TableItemActive.GetCount(); i++)
	{
		SafeDelete(m_TableItemActive[i]);
	}

	for (INT_PTR i = 0; i<m_TimerItemActive.GetCount(); i++)
	{
		SafeDelete(m_TimerItemActive[i]);
	}

	for (INT_PTR i = 0; i<m_TimerItemBuffer.GetCount(); i++)
	{
		SafeDelete(m_TimerItemBuffer[i]);
	}

	//��������
	m_TableItemActive.RemoveAll();
	m_TimerItemActive.RemoveAll();
	m_TimerItemBuffer.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CAndroidUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItem, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItem, Guid, dwQueryVer);
	return NULL;
}

//��ȡ I D
DWORD CAndroidUserItem::GetUserID()
{
	return m_AndroidAccountsInfo.dwUserID;
}

//���Ӻ���
WORD CAndroidUserItem::GetTableID()
{
	//Ч��״̬
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return INVALID_TABLE;

	return m_pIServerUserItem->GetTableID();
}

//���Ӻ���
WORD CAndroidUserItem::GetChairID()
{
	//Ч��״̬
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return INVALID_CHAIR;

	return m_pIServerUserItem->GetChairID();
}

//���ùһ�
VOID  CAndroidUserItem::SetAiHanging(bool bHanging)
{
	if (m_bHanging != bHanging)
	{
		m_bHanging = bHanging;

		//���ùһ�״̬
		if (m_pIServerUserItem)
		{
			m_pIServerUserItem->SetAndroidUserHanging(m_bHanging);
		}
	}
}

//��ȡ���Я���ȼ�����
BYTE CAndroidUserItem::GetRandomCarryGradeIndex()
{
	//��ȡ����
	tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();
	WORD wRandomCarryGrage = AI_CARRY_GRADE_NONE;

	//(0, 1, 2, 3�ֱ��Ӧ�ȼ�A, B, C, D)
	BYTE cbRandCarryGradeIndex = INVALID_BYTE;

	if (pRoomAIParameter)
	{
		//A I ����
		DWORD dwAICountBase = m_pIAndroidUserManager->GetAndroidCount();
		BYTE cbCarryStandardVal = AI_CARRYGRADE_STANDARD;
		cbRandCarryGradeIndex = rand() % AI_CARRY_GRADE;
		bool bRandomValid = false;

		for (int nCirculateIndex = 0; nCirculateIndex < AI_CARRY_GRADE; nCirculateIndex++)
		{
			wRandomCarryGrage = (WORD)pow((float)cbCarryStandardVal, (float)cbRandCarryGradeIndex);
			//ASSERT(wRandomCarryGrage == AI_CARRY_GRADE_A || wRandomCarryGrage == AI_CARRY_GRADE_B|| wRandomCarryGrage == AI_CARRY_GRADE_C || wRandomCarryGrage == AI_CARRY_GRADE_D);

			//��ȡ��Ӧ�ȼ�����
			WORD wAndroidGradeCount = m_pIAndroidUserManager->GetAndroidGradeCount(wRandomCarryGrage);
			WORD wAIScoreFloatRate = (WORD)(((SCORE)(pRoomAIParameter->wAIScoreLevelRate[cbRandCarryGradeIndex] * RANDVAL(pRoomAIParameter->wAIScoreFloatMinRate[cbRandCarryGradeIndex], pRoomAIParameter->wAIScoreFloatMaxRate[cbRandCarryGradeIndex]))) / ((SCORE)100));
			SCORE lAIScoreLevelCount = ((SCORE)(dwAICountBase * wAIScoreFloatRate)) / ((SCORE)100);

			//У������
			if ((wAndroidGradeCount + 1) > (WORD)ceil(lAIScoreLevelCount))
			{
				cbRandCarryGradeIndex = (cbRandCarryGradeIndex + 1) % AI_CARRY_GRADE;
				continue;
			}

			//���û����ȼ�
			m_wAndroidGrade |= wRandomCarryGrage;
			bRandomValid = true;

			break;
		}

		//У����Ч
		if (!bRandomValid)
		{
			//����Ĭ�ϻ����ȼ�(0, 1, 2, 3�ֱ��Ӧ�ȼ�A, B, C, D)
			cbRandCarryGradeIndex = 1;
			wRandomCarryGrage = AI_CARRY_GRADE_B;
			m_wAndroidGrade |= wRandomCarryGrage;
		}
	}

	return cbRandCarryGradeIndex;
}

//��ȡ���н��/��ʼ���
SCORE CAndroidUserItem::GetScoreVariationTimes()
{
	//�û�У��
	//ASSERT(m_pIServerUserItem != NULL);
	if (!m_pIServerUserItem)
	{
		return 0;
	}

	//����У��
	//ASSERT(m_AndroidAccountsInfo.lInitialCarryScore != 0);
	if (m_AndroidAccountsInfo.lInitialCarryScore == 0)
	{
		return 0;
	}

	return (SCORE)(m_pIServerUserItem->GetUserScore() / m_AndroidAccountsInfo.lInitialCarryScore);
}

//���ó�ʼЯ��
void CAndroidUserItem::SetUserItemInitialCarryScore(DWORD dwUserID, SCORE lInitialCarryScore)
{
	if (dwUserID == m_AndroidAccountsInfo.dwUserID)
	{
		m_AndroidAccountsInfo.lInitialCarryScore = lInitialCarryScore;
	}
}

//������Ϸ�仯
void CAndroidUserItem::SetUserItemGameVariationScore(DWORD dwUserID, SCORE lGameVariationScore)
{
	if (dwUserID == m_AndroidAccountsInfo.dwUserID && m_pIServerUserItem && dwUserID == m_pIServerUserItem->GetUserID())
	{
		m_AndroidAccountsInfo.lGameVariationScore += lGameVariationScore;
	}

	//���û�������
	if (!(m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR))
	{
		ModifySwitchTableRate(lGameVariationScore > 0);
	}
}

//�����뿪�ȼ�
void CAndroidUserItem::SetAndroidLeaveGrade(WORD wLeaveGrade)
{
	//ȥ��Ȩλ
	m_wAndroidGrade &= AI_SHIELD_LEAVE_GRADE_MASK;

	//����Ȩλ
	m_wAndroidGrade |= wLeaveGrade;

	//���û����ȼ�
	if (m_pIServerUserItem)
	{
		m_pIServerUserItem->SetAndroidGrade(m_wAndroidGrade);
	}
}

//У��ͬ��
bool CAndroidUserItem::EfficacyAndroidFreeSameTable(WORD wTableID)
{
	//��ʼ����
	if (m_TableItemActive.GetCount() == 0)
	{
		return true;
	}

	//Ч������
	if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR)
	{
		return false;
	}

	//��������
	DWORD dwCurTime = (DWORD)time(NULL);
	DWORD dwMaxSameTableSitDownTickCount = 0;

	//Ѱ������
	for (INT_PTR i = 0; i < m_TableItemActive.GetCount(); i++)
	{
		//��ȡ����
		tagTableItem * pTableItem = m_TableItemActive[i];

		if (pTableItem && pTableItem->wTableID == wTableID && pTableItem->dwSitDownTickCount > dwMaxSameTableSitDownTickCount)
		{
			dwMaxSameTableSitDownTickCount = pTableItem->dwSitDownTickCount;
		}
	}

	if (dwMaxSameTableSitDownTickCount == 0)
	{
		return true;
	}

	if (dwMaxSameTableSitDownTickCount + m_wFreeSameTableSec > dwCurTime)
	{
		return false;
	}

	return true;
}

//�޸Ļ�������
void CAndroidUserItem::ModifySwitchTableRate(bool bAndroidWin)
{
	//
	if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR)
	{
		return;
	}

	//����
	if (bAndroidWin)
	{
		m_wSwitchTableRate = (m_wSwitchTableRate < AI_SWHTABLE_RATE_SUB ? 0 : m_wSwitchTableRate - AI_SWHTABLE_RATE_SUB);
	}
	//����
	else
	{
		m_wSwitchTableRate = (m_wSwitchTableRate + AI_SWHTABLE_RATE_ADD > 100 ? 100 : m_wSwitchTableRate + AI_SWHTABLE_RATE_SUB);
	}
}

//��ȡ�Լ�
IServerUserItem * CAndroidUserItem::GetMeUserItem()
{
	return m_pIServerUserItem;
}

//��Ϸ�û�
IServerUserItem * CAndroidUserItem::GetTableUserItem(WORD wChariID)
{
	//������Ч
	if (GetTableID() == INVALID_TABLE || wChariID == INVALID_CHAIR)
	{
		return NULL;
	}

	//��ȡ�û�
	return m_pIAndroidUserManager->GetUserItem(GetTableID(), wChariID);
}

//��ȡͬ���������
WORD CAndroidUserItem::GetSitRealUserCount()
{
	//��������
	WORD wSitRealUserCount = 0;

	//������Ч
	if (GetTableID() == INVALID_TABLE || GetChairID() == INVALID_CHAIR)
	{
		return wSitRealUserCount;
	}

	//��ȡ��Ϸ����
	tagGameServiceAttrib* pGameServiceAttrib = m_pIAndroidUserManager->GetGameServiceAttrib();
	if (!pGameServiceAttrib)
	{
		return wSitRealUserCount;
	}

	for (WORD wChariID = 0; wChariID < pGameServiceAttrib->wChairCount; wChariID++)
	{
		IServerUserItem * pTempServerUserItem = GetTableUserItem(wChariID);
		if (!pTempServerUserItem || pTempServerUserItem->IsAndroidUser())
		{
			continue;
		}

		wSitRealUserCount++;
	}

	return wSitRealUserCount;
}

//���ͺ���
bool CAndroidUserItem::SendSocketData(WORD wSubCmdID)
{
	//״̬Ч��
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return false;

	//��������
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_GAME, wSubCmdID, NULL, 0);

	return true;
}

//������Ϸ��
bool CAndroidUserItem::PerformSaveScore(SCORE lScore)
{
	//ASSERT(m_pIAndroidUserManager != NULL);
	if (m_pIAndroidUserManager->GetGameServiceOption()->wServerType == GAME_GENRE_MATCH)
		return true;

	//״̬Ч��
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return false;

	//Ч����Ϸ��
	//ASSERT(m_pIServerUserItem->GetUserScore() >= lScore);
	if (m_pIServerUserItem->GetUserScore() < lScore) return false;

	//�޸���Ϸ��
	m_pIServerUserItem->ModifyUserInsure(-lScore, 0L, 0L);

	return true;
}

//��ȡ��Ϸ��
bool CAndroidUserItem::PerformTakeScore(SCORE lScore)
{
	//ASSERT(m_pIAndroidUserManager != NULL);
	if (m_pIAndroidUserManager->GetGameServiceOption()->wServerType == GAME_GENRE_MATCH)
		return true;

	//״̬Ч��
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return false;

	//�޸���Ϸ��
	m_pIServerUserItem->ModifyUserInsure(lScore, 0L, 0L);

	return true;
}

//���ͺ���
bool CAndroidUserItem::SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return false;

	//��������
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_GAME, wSubCmdID, pData, wDataSize);

	return true;
}

//��������
bool CAndroidUserItem::JudgeAndroidActionAndRemove(WORD wAction)
{
	return (m_wAndroidAction & wAction) != 0;
}

//�����ȼ�
bool CAndroidUserItem::JudgeAndroidGrade(WORD wAndroidGrade)
{
	//��������
	bool bResult = (m_wAndroidGrade&wAndroidGrade) != 0;

	return bResult;
}

//ɾ��ʱ��
bool CAndroidUserItem::KillGameTimer(UINT nTimerID)
{
	//ɾ��ʱ��
	if (nTimerID != 0)
	{
		//Ѱ������
		for (INT_PTR i = 0; i<m_TimerItemActive.GetCount(); i++)
		{
			//��ȡʱ��
			tagTimerItem * pTimerItem = m_TimerItemActive[i];

			//ɾ���ж�
			if (pTimerItem->nTimerID == nTimerID)
			{
				m_TimerItemActive.RemoveAt(i);
				m_TimerItemBuffer.Add(pTimerItem);

				return true;
			}
		}
	}
	else
	{
		m_TimerItemBuffer.Append(m_TimerItemActive);
		m_TimerItemActive.RemoveAll();
	}

	return false;
}

//����ʱ��
bool CAndroidUserItem::SetGameTimer(UINT nTimerID, UINT nElapse)
{
	//Ѱ������
	for (INT_PTR i = 0; i<m_TimerItemActive.GetCount(); i++)
	{
		//��ȡʱ��
		tagTimerItem * pTimerItem = m_TimerItemActive[i];

		//�����ж�
		if (pTimerItem->nTimerID == nTimerID)
		{
			pTimerItem->nTimeLeave = nElapse;
			return true;
		}
	}

	//��������
	tagTimerItem * pTimerItem = NULL;
	WORD wStorageCount = (WORD)m_TimerItemBuffer.GetCount();

	//�������
	if (wStorageCount>0)
	{
		//��ȡ����
		pTimerItem = m_TimerItemBuffer[wStorageCount - 1];

		//��������
		m_TimerItemActive.Add(pTimerItem);
		m_TimerItemBuffer.RemoveAt(wStorageCount - 1);
	}

	//��������
	if (pTimerItem == NULL)
	{
		try
		{
			//��������
			pTimerItem = new tagTimerItem;
			if (pTimerItem == NULL) return false;

			//��������
			m_TimerItemActive.Add(pTimerItem);
		}
		catch (...)
		{
			//ASSERT(FALSE);
			return false;
		}
	}

	//���ñ���
	pTimerItem->nTimerID = nTimerID;
	pTimerItem->nTimeLeave = nElapse;

	return true;
}

//����׼��
bool CAndroidUserItem::SendUserReady(VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	//	//ASSERT((m_pIServerUserItem!=NULL)&&(m_pIServerUserItem->GetUserStatus()==US_SIT));
	if ((m_pIServerUserItem == NULL) || (m_pIServerUserItem->GetUserStatus() != US_SIT)) return false;

	//����׼��
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_FRAME, SUB_GF_USER_READY, pData, wDataSize);

	return true;
}

//������־
void CAndroidUserItem::SendLogData(const TCHAR * pszFormat, ...)
{
	TCHAR buf[1024];
	ZeroMemory(buf, 1024 * sizeof(TCHAR));

	va_list arglist;
	va_start(arglist, pszFormat);
	int nBuffSize = _tcslen(buf);
	vswprintf_s(buf, 1024, pszFormat, arglist);
	va_end(arglist);

	m_pIAndroidUserManager->SendLogData(buf);
}

//������־����
void CAndroidUserItem::SendLogData(tagLogUserInfo LogUserInfo)
{
	m_pIAndroidUserManager->SendLogData(LogUserInfo);
}

//��������
bool CAndroidUserItem::SendChatMessage(DWORD dwTargetUserID)
{
	//Ч����������
	//ASSERT(m_wAndroidChatType != ANDROID_INVALID_CT);
	if (m_wAndroidChatType == ANDROID_INVALID_CT)
	{
		return false;
	}

	//��������
	WORD wAndroidChatType = m_wAndroidChatType;
	m_wAndroidChatType = ANDROID_INVALID_CT;

	//Ч���û�
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL)
	{
		return false;
	}

	//Ч������
	if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR)
	{
		return false;
	}

	//��ȡ��������
	tagGameChatInfo * pGameChatInfo = m_pIAndroidUserManager->GetGameChatInfo();

	if (!pGameChatInfo)
	{
		return false;
	}

	//Ч��������Ŀ
	if (pGameChatInfo->cbGameChatCount <= 0)
	{
		return false;
	}

	//Ч������
	if (GetSitRealUserCount() == 0)
	{
		return false;
	}

	//Ч���������ʱ��
	if (EfficacyFreeChatMessage() == false)
	{
		return false;
	}

	//Ч�����
	bool bSendChatMessage = false;
	TCHAR szGameChatString[MAX_CHATSTRINGLEN];
	ZeroMemory(szGameChatString, sizeof(szGameChatString));
	BYTE cbGameChatCount = pGameChatInfo->cbGameChatCount;
	WORD wRandFirstIndex = rand() % cbGameChatCount;

	for (WORD i = 0; i < cbGameChatCount; i++)
	{
		if (pGameChatInfo->GameChatItemInfo[wRandFirstIndex].wChatTypeIndex == wAndroidChatType)
		{
			if (DETECTIONPROBABILITY(pGameChatInfo->GameChatItemInfo[wRandFirstIndex].cbChatChance))
			{
				bSendChatMessage = true;
				CopyMemory(szGameChatString, pGameChatInfo->GameChatItemInfo[wRandFirstIndex].szGameChatString, sizeof(szGameChatString));
			}

			break;
		}

		wRandFirstIndex = (wRandFirstIndex + 1) % cbGameChatCount;
	}

	//Ч��ɹ�
	if (bSendChatMessage)
	{
		//������Ϣ
		CMD_GF_C_UserChat UserChat;
		lstrcpyn(UserChat.szChatString, szGameChatString, CountArray(UserChat.szChatString));

		//��������
		UserChat.dwChatColor = 16777215;
		UserChat.dwTargetUserID = dwTargetUserID;
		UserChat.wChatLength = CountStringBuffer(UserChat.szChatString);

		//��������
		DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
		WORD wSendSize = sizeof(UserChat)-sizeof(UserChat.szChatString) + UserChat.wChatLength*sizeof(TCHAR);
		m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_FRAME, SUB_GF_USER_CHAT, &UserChat, wSendSize);

		//����ʱ��
		m_dwLastChatMessageTickCount = (DWORD)time(NULL);
	}

	return true;
}

//���ͱ���
bool CAndroidUserItem::SendUserExpression(DWORD dwTargetUserID, WORD wItemIndex)
{
	//Ч���û�
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL)
	{
		return false;
	}

	//������Ϣ
	CMD_GF_C_UserExpression UserExpression;
	ZeroMemory(&UserExpression, sizeof(UserExpression));

	//��������
	UserExpression.wItemIndex = wItemIndex;
	UserExpression.dwTargetUserID = dwTargetUserID;

	//��������
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_FRAME, SUB_GF_USER_EXPRESSION, &UserExpression, sizeof(UserExpression));

	return true;
}

//ʱ���¼�
bool CAndroidUserItem::OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//Ѱ������
	for (INT_PTR i = m_TimerItemActive.GetCount() - 1; i >= 0; i--)
	{
		//��������
		tagTimerItem * pTimerItem = m_TimerItemActive[i];

		//ʱ�䴦��
		if (pTimerItem->nTimeLeave <= 1L)
		{
			//��������
			m_TimerItemActive.RemoveAt(i);
			m_TimerItemBuffer.Add(pTimerItem);

			try
			{
				if (pTimerItem->nTimerID == IDI_ANDROID_LOGONHANGING)
				{
					//Ч������
					if (!(m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR))
					{
						continue;
					}

					//Я���ȼ�����
					BYTE cbCarryGradeIndex = GetCarryGradeIndex();
					tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();

					//Я���ȼ�D�����ǹһ�
					if (cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1 && pRoomAIParameter != NULL && !IsAiHanging())
					{
						//��������
						bool bAndroidOccupy = (m_wAndroidAction&ANDROID_OCCUPYSEAT) != 0;
						bool bAndroidBeBanker = (m_wAndroidAction&ANDROID_BEBANKER) != 0;

						//����AIռλ��AIׯ��
						if (DETECTIONPROBABILITY(pRoomAIParameter->wSetHangingRate[cbCarryGradeIndex]) && !bAndroidOccupy && !bAndroidBeBanker)
						{
							bool bTempHanging = true;

							//�����һ�״̬
							bool bSucceed = RectifyHangingStatus(bTempHanging);

							if (bTempHanging != m_bHanging)
							{
								SetAiHanging(true);
							}
						}

						SetGameTimer(IDI_ANDROID_CHECKHANGING, pRoomAIParameter->wMinuteChangeStatus * 60);
					}
				}
				else if (pTimerItem->nTimerID == IDI_ANDROID_CHECKHANGING)
				{
					//Ч������
					if (!(m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR))
					{
						continue;
					}

					//Я���ȼ�����
					BYTE cbCarryGradeIndex = GetCarryGradeIndex();
					tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();

					//Я���ȼ�D�����ǹһ�
					if (cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1 && pRoomAIParameter != NULL)
					{
						//50%�ָ�,50%�����һ�
						if (DETECTIONPROBABILITY(50) && IsAiHanging())
						{
							bool bTempHanging = false;

							//�����һ�״̬
							bool bSucceed = RectifyHangingStatus(bTempHanging);

							if (bTempHanging != m_bHanging)
							{
								SetAiHanging(false);
							}
						}

						SetGameTimer(IDI_ANDROID_CHECKHANGING, pRoomAIParameter->wMinuteChangeStatus * 60);
					}
				}
				else
				{
					//ʱ��֪ͨ
					if (m_pIAndroidUserItemSink != NULL)
					{
						m_pIAndroidUserItemSink->OnEventTimer(pTimerItem->nTimerID);
					}
				}
			}
			catch (...)
			{
				CString strMessage;
				strMessage.Format(TEXT("OnTimerPulse: TimerID=%d"), pTimerItem->nTimerID);
				CTraceService::TraceString(strMessage, TraceLevel_Exception);
			}
		}
		else
		{
			//���ñ���
			pTimerItem->nTimeLeave--;
		}
	}


	return true;
}

//������Ϣ
bool CAndroidUserItem::OnSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��¼ʧ��
	if ((wMainCmdID == MDM_GR_LOGON) && (wSubCmdID == SUB_GR_LOGON_FAILURE))
	{
		return OnSocketSubLogonFailure(pData, wDataSize);
	}

	//��¼���
	if ((wMainCmdID == MDM_GR_LOGON) && (wSubCmdID == SUB_GR_LOGON_FINISH))
	{
		return OnSocketSubLogonFinish(pData, wDataSize);
	}

	//�û�״̬
	if ((wMainCmdID == MDM_GR_USER) && (wSubCmdID == SUB_GR_USER_STATUS))
	{
		return OnSocketSubUserStatus(pData, wDataSize);
	}

	//��Ϸ��Ϣ
	if ((wMainCmdID == MDM_GF_GAME) || (wMainCmdID == MDM_GF_FRAME))
	{
		return OnSocketGameMessage(wMainCmdID, wSubCmdID, pData, wDataSize);
	}

	//ϵͳ��Ϣ
	if ((wMainCmdID == MDM_CM_SYSTEM) && (wSubCmdID == SUB_CM_SYSTEM_MESSAGE))
	{
		return OnSocketSubSystemMessage(pData, wDataSize);
	}

	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItem::OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��Ϸ����
		if ((wMainCmdID == MDM_GF_GAME) && (m_pIAndroidUserItemSink != NULL))
		{
			return m_pIAndroidUserItemSink->OnEventGameMessage(wSubCmdID, pData, wDataSize);
		}

		//��ܴ���
		if (wMainCmdID == MDM_GF_FRAME)
		{
			//Ĭ�ϴ���
			switch (wSubCmdID)
			{
			case SUB_GF_GAME_STATUS:		//��Ϸ״̬
			{
												//Ч�����
												//ASSERT(wDataSize == sizeof(CMD_GF_GameStatus));
												if (wDataSize != sizeof(CMD_GF_GameStatus)) return false;

												//��Ϣ����
												CMD_GF_GameStatus * pGameStatus = (CMD_GF_GameStatus *)pData;

												//���ñ���
												m_cbGameStatus = pGameStatus->cbGameStatus;

												return true;
			}
			case SUB_GF_GAME_SCENE:			//��Ϸ����
			{
												//��Ϸ����
												if (m_pIAndroidUserItemSink != NULL)
												{
													bool bLookonUser = (m_pIServerUserItem->GetUserStatus() == US_LOOKON);
													return m_pIAndroidUserItemSink->OnEventSceneMessage(m_cbGameStatus, bLookonUser, pData, wDataSize);
												}
												else
												{
													//���Ϳ�ʼ
													IServerUserItem * pIServerUserItem = GetMeUserItem();
													if (pIServerUserItem->GetUserStatus() != US_READY) SendUserReady(NULL, 0);
												}

												return true;
			}
			case SUB_GF_USER_READY:			//�û�׼��
			{
												if (m_pIServerUserItem->GetUserStatus() >= US_READY)
													return true;

												//���Ϳ�ʼ
												SendUserReady(NULL, 0);
												return true;
			}
			default:
			{
					   if (m_pIAndroidUserItemSink != NULL)
						   return m_pIAndroidUserItemSink->OnEventFrameMessage(wSubCmdID, pData, wDataSize);
					   break;
			}
			}
		}
	}
	catch (...)
	{
		CString strMessage;
		strMessage.Format(TEXT("OnSocketGameMessage: MainID=%d\tSubID=%d"), wMainCmdID, wSubCmdID);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);
	}

	return true;
}

//��¼ʧ��
bool CAndroidUserItem::OnSocketSubLogonFailure(VOID * pData, WORD wDataSize)
{
	return true;
}

//��¼���
bool CAndroidUserItem::OnSocketSubLogonFinish(VOID * pData, WORD wDataSize)
{
	//���ñ���
	m_cbGameStatus = GAME_STATUS_FREE;
	m_pIServerUserItem = m_pIServerUserManager->SearchUserItem(m_AndroidAccountsInfo.dwUserID);

	//״̬Ч��
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return false;

	//�û�״̬
	m_CurrentUserStatus.wChairID = m_pIServerUserItem->GetChairID();
	m_CurrentUserStatus.wTableID = m_pIServerUserItem->GetTableID();
	m_CurrentUserStatus.cbUserStatus = m_pIServerUserItem->GetUserStatus();

	//�������
	CMD_GR_UserRule UserRule;
	ZeroMemory(&UserRule, sizeof(UserRule));

	//���͹���
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GR_USER, SUB_GR_USER_RULE, &UserRule, sizeof(UserRule));

	//����ʱ��
	m_dwStandupTickCount = (DWORD)time(NULL);
	m_dwLogonFinishTickCount = (DWORD)time(NULL);
	m_dwReposeTickCount = m_dwMinSitInterval + rand() % (__max(m_dwMaxSitInterval - m_dwMinSitInterval, 1));

	//�����ж�
	if (m_pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		StartGameClient();
	}

	//�һ���ʱ
	if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR)
	{
		//Я���ȼ�����
		BYTE cbCarryGradeIndex = GetCarryGradeIndex();
		tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();

		//Я���ȼ�D�����ǹһ�
		if (cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1 && pRoomAIParameter != NULL)
		{
			WORD wRandMin = RANDVAL(pRoomAIParameter->wHangingEnterMinTime[cbCarryGradeIndex], pRoomAIParameter->wHangingEnterMaxTime[cbCarryGradeIndex]);

			SetGameTimer(IDI_ANDROID_LOGONHANGING, wRandMin * 60);
		}
	}
	else
	{
		//���û�������
		m_wSwitchTableRate = RANDVAL(AI_INI_SWHTABLE_RATE_MIN, AI_INI_SWHTABLE_RATE_MAX);
		m_wFreeSameTableSec = RANDVAL(AI_FREE_SAMETABLE_SECMIN, AI_FREE_SAMETABLE_SECMAX);
	}

	return true;
}

//�û�״̬
bool CAndroidUserItem::OnSocketSubUserStatus(VOID * pData, WORD wDataSize)
{
	//Ч�����
	//ASSERT(wDataSize == sizeof(CMD_GR_UserStatus));
	if (wDataSize<sizeof(CMD_GR_UserStatus)) return false;

	//��������
	CMD_GR_UserStatus * pUserStatus = (CMD_GR_UserStatus *)pData;

	//����ж�
	if (pUserStatus->dwUserID != m_AndroidAccountsInfo.dwUserID)
	{
		return true;
	}

	//�û�״̬
	tagUserStatus LastUserStatus;
	CopyMemory(&LastUserStatus, &m_CurrentUserStatus, sizeof(LastUserStatus));
	CopyMemory(&m_CurrentUserStatus, &pUserStatus->UserStatus, sizeof(m_CurrentUserStatus));

	//��ʼ�л�
	if ((LastUserStatus.cbUserStatus != US_READY) && (m_CurrentUserStatus.cbUserStatus == US_READY))
	{
	}

	//��������
	if (m_CurrentUserStatus.cbUserStatus == US_FREE)
	{
		//���þ���
		m_wAndroidAction = 0;
		m_wAndroidChatType = ANDROID_INVALID_CT;

		//����ʱ��
		m_dwStandupTickCount = (DWORD)time(NULL);
		m_dwReposeTickCount = m_dwMinSitInterval + rand() % (__max(m_dwMaxSitInterval - m_dwMinSitInterval, 1));

		m_dwLastChatMessageTickCount = 0;

		//���û�������
		m_wSwitchTableRate = RANDVAL(AI_INI_SWHTABLE_RATE_MIN, AI_INI_SWHTABLE_RATE_MAX);
	}

	//����״̬
	if (m_CurrentUserStatus.cbUserStatus == US_SIT)
	{
		//ASSERT(m_CurrentUserStatus.wTableID != INVALID_TABLE && m_CurrentUserStatus.wChairID != INVALID_CHAIR);

		//��������
		tagTableItem * pTableItem = new tagTableItem;
		ZeroMemory(pTableItem, sizeof(tagTableItem));

		pTableItem->wTableID = m_CurrentUserStatus.wTableID;
		pTableItem->dwSitDownTickCount = (DWORD)time(NULL);

		//��������
		m_TableItemActive.Add(pTableItem);
	}

	//��Ϸ״̬
	if (m_CurrentUserStatus.cbUserStatus == US_PLAYING)
	{
		if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount != MAX_CHAIR)
		{
			//��������
			if (DETECTIONPROBABILITY(m_wSwitchTableRate))
			{
				m_wAndroidAction |= ANDROID_WAITSTANDUP;

				//������������
				m_wAndroidChatType = ANDROID_STANDUP_CT;

				SendChatMessage(0);
			}
		}
	}

	//�ر��ж�
	if ((m_bStartClient == true) && (m_CurrentUserStatus.wTableID == INVALID_TABLE))
	{
		//�ر���Ϸ
		CloseGameClient();

		return true;
	}

	//�����ж�
	if ((m_bStartClient == false) && (m_CurrentUserStatus.wTableID != INVALID_TABLE))
	{
		//������Ϸ
		StartGameClient();

		return true;
	}

	return true;
}

//ϵͳ��Ϣ
bool CAndroidUserItem::OnSocketSubSystemMessage(VOID * pData, WORD wDataSize)
{
	//��������
	CMD_CM_SystemMessage * pSystemMessage = (CMD_CM_SystemMessage *)pData;
	WORD wHeadSize = sizeof(CMD_CM_SystemMessage)-sizeof(pSystemMessage->szString);

	//Ч�����
	//ASSERT((wDataSize>wHeadSize) && (wDataSize == (wHeadSize + pSystemMessage->wLength*sizeof(TCHAR))));
	if ((wDataSize <= wHeadSize) || (wDataSize != (wHeadSize + pSystemMessage->wLength*sizeof(TCHAR)))) return false;

	//�رմ���
	if ((pSystemMessage->wType&(SMT_CLOSE_ROOM | SMT_CLOSE_LINK)) != 0)
	{
		//�رմ���
		DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
		m_pIAndroidUserManager->DeleteAndroidUserItem(dwAndroidID, true);
	}

	return true;
}

//������Ϸ
VOID CAndroidUserItem::StartGameClient()
{
	//״̬�ж�
	//ASSERT((m_bStartClient == false) && (m_pIServerUserItem != NULL));
	//ASSERT((m_pIServerUserItem->GetTableID() != INVALID_TABLE) && (m_pIServerUserItem->GetChairID() != INVALID_CHAIR));

	//���ñ���
	m_bStartClient = true;

	//��������
	CMD_GF_GameOption GameOption;
	ZeroMemory(&GameOption, sizeof(GameOption));

	//��������
	GameOption.cbAllowLookon = FALSE;
	GameOption.dwFrameVersion = INVALID_DWORD;
	GameOption.dwClientVersion = INVALID_DWORD;

	//��������
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_FRAME, SUB_GF_GAME_OPTION, &GameOption, sizeof(GameOption));

	//���ö���
	m_wAndroidAction |= ANDROID_ENTERGAME;

	return;
}

//�ر���Ϸ
VOID CAndroidUserItem::CloseGameClient()
{
	//״̬�ж�
	//ASSERT((m_bStartClient == true) && (m_pIServerUserItem != NULL));
	//ASSERT((m_pIServerUserItem->GetTableID() == INVALID_TABLE) && (m_pIServerUserItem->GetChairID() == INVALID_CHAIR));

	//ɾ��ʱ��
	KillGameTimer(0);

	//���ñ���
	m_bStartClient = false;
	m_cbGameStatus = GAME_STATUS_FREE;

	//��Ϸ��λ
	if (m_pIAndroidUserItemSink != NULL)
	{
		m_pIAndroidUserItemSink->RepositionSink();
	}

	return;
}

//��λ����
VOID CAndroidUserItem::RepositUserItem()
{
	//ɾ����ʱ
	if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR)
	{
		//Я���ȼ�����
		BYTE cbCarryGradeIndex = GetCarryGradeIndex();
		tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();

		//Я���ȼ�D�����ǹһ�
		if (cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1 && pRoomAIParameter != NULL)
		{
			KillGameTimer(IDI_ANDROID_LOGONHANGING);
			KillGameTimer(IDI_ANDROID_CHECKHANGING);
		}
	}

	//״̬����
	m_bWaitLeave = false;
	m_bStartClient = false;
	m_wAndroidAction = 0;
	m_wAndroidGrade = AI_CARRY_GRADE_NONE | AI_LEAVE_GRADE_NONE;
	m_wAndroidChatType = ANDROID_INVALID_CT;
	m_cbGameStatus = GAME_STATUS_FREE;

	//�û�״̬
	m_CurrentUserStatus.cbUserStatus = US_NULL;
	m_CurrentUserStatus.wChairID = INVALID_CHAIR;
	m_CurrentUserStatus.wTableID = INVALID_TABLE;

	m_bHanging = false;
	m_bInitialIn = false;
	m_wSwitchTableRate = 0;
	m_wFreeSameTableSec = 0;

	//ɾ���洢
	for (INT_PTR i = 0; i<m_TableItemActive.GetCount(); i++)
	{
		SafeDelete(m_TableItemActive[i]);
	}

	m_TableItemActive.RemoveAll();

	//�󶨱���
	m_pIServerUserItem = NULL;

	//ʱ�����
	m_dwMinSitInterval = 0;
	m_dwMaxSitInterval = 0;
	m_dwStandupTickCount = 0;
	m_dwReposeTickCount = 0;
	m_dwLastChatMessageTickCount = 0;
	m_dwLogonFinishTickCount = 0;

	//״̬��Ϣ
	ZeroMemory(&m_AndroidAccountsInfo, sizeof(m_AndroidAccountsInfo));

	//ɾ��ʱ��
	m_TimerItemBuffer.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	//��������
	m_wRoundID = __max(1, m_wRoundID + 1);

	//��λ��Ϸ
	if (m_pIAndroidUserItemSink != NULL)
	{
		m_pIAndroidUserItemSink->RepositionSink();
	}

	return;
}

//Ч���������ʱ��
bool CAndroidUserItem::EfficacyFreeChatMessage()
{
	//������Ч
	if (GetTableID() == INVALID_TABLE || GetChairID() == INVALID_CHAIR)
	{
		return NULL;
	}

	//ʱ�����
	DWORD dwFreeChatMsg = rand() % (INDIVIDUAL_FREE_CHATMSG_MAX - INDIVIDUAL_FREE_CHATMSG_MIN) + INDIVIDUAL_FREE_CHATMSG_MIN;
	DWORD dwIntervalChatMsg = rand() % (INTERVAL_CHATMSG_MAX - INTERVAL_CHATMSG_MIN) + INTERVAL_CHATMSG_MIN;

	//��Ϣʱ��
	DWORD dwCurTime = (DWORD)time(NULL);

	//Ч���������
	if (m_dwLastChatMessageTickCount + dwFreeChatMsg > dwCurTime)
	{
		return false;
	}

	//ö�ٻ���
	WORD wEnumIndex = 0;
	IAndroidUserItem * pITempAndroidUserItem = NULL;
	do
	{
		pITempAndroidUserItem = m_pIAndroidUserManager->EnumAndroidUserItem(wEnumIndex++);
		if (!pITempAndroidUserItem)
		{
			break;
		}

		if (!pITempAndroidUserItem->GetMeUserItem() || pITempAndroidUserItem->GetTableID() != GetTableID() || pITempAndroidUserItem->GetUserID() == GetUserID())
		{
			continue;
		}

		DWORD dwLastChatMessageTickCount = pITempAndroidUserItem->GetLastChatMessageTickCount();

		if (dwLastChatMessageTickCount + dwIntervalChatMsg > dwCurTime)
		{
			return false;
		}

	} while (pITempAndroidUserItem != NULL);

	return true;
}

//�����һ�״̬
bool CAndroidUserItem::RectifyHangingStatus(bool& bHanging)
{
	//������Ч
	if (GetTableID() == INVALID_TABLE || GetChairID() == INVALID_CHAIR || m_pIServerUserItem == NULL)
	{
		return false;
	}

	//Ч������
	if (!(m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR))
	{
		return false;
	}

	//Ч��Я���ȼ�
	BYTE cbCarryGradeIndex = GetCarryGradeIndex();
	if (!(cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1))
	{
		return false;
	}

	//��ȡA I ���� 
	tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();

	if (pRoomAIParameter)
	{
		//�һ�����
		WORD wAIHangingRate = RANDVAL(pRoomAIParameter->wAIHangingMinRate, pRoomAIParameter->wAIHangingMaxRate);

		//������Ŀ
		WORD wAndroidCount = m_pIAndroidUserManager->GetAndroidCount();
		WORD wCurAndroidHangingCount = m_pIAndroidUserManager->GetAndroidHangingCount();
		WORD wCurAndroidInHangingCount = wAndroidCount - wCurAndroidHangingCount;

		SCORE lKeyAndroidHangingCount = ((SCORE)(wAndroidCount * wAIHangingRate)) / ((SCORE)100);
		WORD wKeyAndroidHangingCount = (WORD)ceil(lKeyAndroidHangingCount);
		WORD wKeyAndroidInHangingCount = wAndroidCount - wKeyAndroidHangingCount;

		//�һ�Ч��
		if (bHanging)
		{
			//У������
			if ((wCurAndroidHangingCount + 1) <= wKeyAndroidHangingCount)
			{
				return true;
			}
			else
			{
				bHanging = false;
				return true;
			}
		}
		else
		{
			//У������
			if ((wCurAndroidInHangingCount + 1) <= wKeyAndroidInHangingCount)
			{
				return true;
			}
			else
			{
				bHanging = true;
				return true;
			}
		}
	}

	return false;
}

//Я���ȼ�����
BYTE CAndroidUserItem::GetCarryGradeIndex()
{
	BYTE cbCarryStandardVal = AI_CARRYGRADE_STANDARD;
	WORD wCarryGrade = m_wAndroidGrade & AI_CARRY_GRADE_MASK;

	//Я���ȼ�����(0,1,2,3�ֱ��Ӧ�ȼ�A,B,C,D)
	BYTE cbCarryGradeIndex = sqrtEx((float)cbCarryStandardVal, (float)wCarryGrade);

	//ASSERT(cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE);

	return cbCarryGradeIndex;
}

//�뿪�ȼ�����
BYTE CAndroidUserItem::GetLeaveGradeIndex()
{
	BYTE cbLeaveStandardVal = AI_LEAVEGRADE_STANDARD;
	WORD wLeaveGrade = m_wAndroidGrade & AI_LEAVE_GRADE_MASK;
	wLeaveGrade >>= 4;

	//�뿪�ȼ�����(0,1,2�ֱ��Ӧ�޵ȼ�������,������,�������)
	BYTE cbLeaveGradeIndex = sqrtEx((float)cbLeaveStandardVal, (float)wLeaveGrade);

	//ASSERT(cbLeaveGradeIndex >= 0 && cbLeaveGradeIndex < AI_LEAVE_GRADE);

	return cbLeaveGradeIndex;
}
//////////////////////////////////////////////////////////////////////////////////
